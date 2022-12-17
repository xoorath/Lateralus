module;

#if IMGUI_SUPPORT
#include <imgui.h>
#endif

#include <Core.h>
//#include <Core.Log.h>

#if USE_GLFW_WINDOW
#include <GL/glew.h>
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#endif

export module Lateralus.Platform.Window.GLFW;

#if USE_GLFW_WINDOW

import Lateralus.Platform.Error;
#if IMGUI_SUPPORT
import Lateralus.Platform.ImGui.Impl;
import Lateralus.Platform.ImGui.GLFW;
import Lateralus.Platform.ImGui.OpenGL;
import Lateralus.Platform.ImGui.Theme;
#endif
import Lateralus.Platform.Input;
import Lateralus.Platform.Input.GLFW;
import Lateralus.Platform.Platform;
import Lateralus.Platform.Window;
import Lateralus.Core;

import <atomic>;
import <format>;
import <mutex>;
import <optional>;
import <string_view>;
import <vector>;

using namespace std;
using namespace std::string_view_literals;

using namespace Lateralus::Core;
#if IMGUI_SUPPORT
using namespace Lateralus::Platform::ImGui;
#endif
using namespace Lateralus::Platform::Input;
using namespace Lateralus::Platform::Input::GLFW;

namespace Lateralus::Platform::GLFW
{
namespace
{
constexpr uint32 k_OpenGLVersionMajor = 3;
constexpr uint32 k_OpenGLVersionMinor = 2;
char const *k_GlslVersion = "#version 150";
} // namespace

export class Window : public iWindow
{
public:
    ~Window() override { TryShutdown(); }

    // iWindow
    optional<Error> Create(WindowCreateContext const &ctx) override
    {
        if (m_Window != nullptr)
        {
            if (auto err = TryShutdown(); err.has_value())
            {
                return err;
            }
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, k_OpenGLVersionMajor);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, k_OpenGLVersionMinor);

        {
            uint32 const width = ctx.GetWidth();
            uint32 const height = ctx.GetHeight();
            string const title = ctx.GetTitle();
            // The monitor to use for full screen mode, or NULL for windowed mode.
            GLFWmonitor *monitor = nullptr;
            // The window whose context to share resources with, or NULL to not share resources.
            GLFWwindow *windowShare = nullptr;

            m_Window = glfwCreateWindow(width, height, title.c_str(), monitor, windowShare);
        }

        if (m_Window == nullptr)
        {
            return Error("Failed to create GLFWWindow");
        }

        glfwMakeContextCurrent(m_Window);
        // enable vsync
        glfwSwapInterval(1);

        if (GLenum result = glewInit(); result != GLEW_OK)
        {
            return Error(format("Couldn't init glew. Result: {}", result));
        }

        {
            shared_ptr<InputProvider> inputProvider = make_shared<InputProvider>();
            if (auto err = inputProvider->Init(m_Window); err.has_value())
            {
                return err;
            }
            m_Input = move(inputProvider);
        }

#if IMGUI_SUPPORT
        // do/while to break after logging an error.
        // We log an error here instead of returning an error because imgui failing is recoverable.
        do
        {
            namespace ImGui = ::ImGui;
            if (!IMGUI_CHECKVERSION())
            {
                //LOG_ERROR("Could not init imgui: IMGUI_CHECKVERSION()");
                break;
            }

            m_ImGuiContext = ImGui::CreateContext();
            if (m_ImGuiContext == nullptr)
            {
                //LOG_ERROR("Could not init imgui: CreateContext()");
                break;
            }

            ApplyTheme();

            shared_ptr<ImplGLFW> implGlfw;
            {
                implGlfw = make_shared<ImplGLFW>();
                if (auto err = implGlfw->Init(m_Window, m_Input); err.has_value())
                {
                    //LOG_ERROR("Could not init imgui: implGlfw->Init() {}", err.value().GetErrorMessage());
                    break;
                }
                m_Impls.push_back(move(implGlfw));
            }

            {
                shared_ptr<ImplOpenGL> implOpenGL = make_shared<ImplOpenGL>();
                if (auto err = implOpenGL->Init(k_GlslVersion); err.has_value())
                {
                    //LOG_ERROR("Could not init imgui: implOpenGL->Init() {}", err.value().GetErrorMessage());
                    break;
                }
                m_Impls.push_back(move(implOpenGL));
            }

        } while (false);
#endif

        int32 screenWidth, screenHeight;
        glfwGetFramebufferSize(m_Window, &screenWidth, &screenHeight);
        glViewport(0, 0, static_cast<int>(screenWidth), static_cast<int>(screenHeight));

        return Success;
    }

    // iWindow
    bool ShouldClose() const override { return glfwWindowShouldClose(m_Window); }

    // iWindow
    void PollEvents() override { glfwPollEvents(); }

    // iWindow
    void Clear() override
    {
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    // iWindow
    void NewFrame() override
    {
#if IMGUI_SUPPORT
        // feed inputs to dear imgui, start new frame
        for (auto const &impl : m_Impls)
        {
            impl->NewFrame();
        }
        ::ImGui::NewFrame();
#endif
    }

    // iWindow
    void Render() override
    {
#if IMGUI_SUPPORT
        ::ImGui::EndFrame();
        ::ImGui::Render();
        for (auto const &impl : m_Impls)
        {
            impl->Render();
        }
#endif
    }

    // iWindow
    void SwapBuffers() override
    {
        int32 screenWidth, screenHeight;
        glfwGetFramebufferSize(m_Window, &screenWidth, &screenHeight);
        glViewport(0, 0, static_cast<int>(screenWidth), static_cast<int>(screenHeight));

        glfwSwapBuffers(m_Window);
    }

private:
    optional<Error> TryMakeContextCurrent()
    {
        if (m_Window == nullptr)
        {
            return Error("Unable to make window current: window missing.");
        }
        glfwMakeContextCurrent(m_Window);

#if IMGUI_SUPPORT
        if (m_ImGuiContext != nullptr)
        {
            ::ImGui::SetCurrentContext(m_ImGuiContext);
        }
#endif

        return Success;
    }

    optional<Error> TryGetFrameBufferSize(int32 &outWidth, int32 &outHeight)
    {
        if (m_Window == nullptr)
        {
            return Error("Unable to get frame buffer size: window missing.");
        }
        int framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize(m_Window, &framebufferWidth, &framebufferHeight);
        outWidth = static_cast<int32>(framebufferWidth);
        outHeight = static_cast<int32>(framebufferHeight);
        return Success;
    }

    optional<Error> TryShutdown()
    {
        optional<Error> problems = Success;

        if (m_Window == nullptr)
        {
            problems = Error("Unable to destroy window. Window missing.");
        }
        else
        {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }

#if IMGUI_SUPPORT
        for (auto &impl : m_Impls)
        {
            impl->Shutdown();
        }
        m_Impls.clear();

        if (m_ImGuiContext == nullptr)
        {
            problems = Error("Unable to shutdown imgui: imgui context missing.");
        }
        else
        {
            ::ImGui::DestroyContext(m_ImGuiContext);
            m_ImGuiContext = nullptr;
        }
#endif
        if (m_Input != nullptr)
        {
            m_Input->Shutdown();
            m_Input.reset();
        }

        return problems;
    }

    GLFWwindow *m_Window = nullptr;
    shared_ptr<Input::iInputProvider> m_Input;

#if IMGUI_SUPPORT
    ImGuiContext *m_ImGuiContext = nullptr;
    vector<shared_ptr<iImpl>> m_Impls;
#endif
};
} // namespace Lateralus::Platform::GLFW

#endif