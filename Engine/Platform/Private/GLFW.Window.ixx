module;

#if PLATFORM_WIN64

#if IMGUI_SUPPORT
#include <imgui.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <Core.Log.h>

export module Lateralus.Platform.GLFW.Window;

import Lateralus.Platform.Error;
import Lateralus.Platform.Imgui.GLFW;
import Lateralus.Platform.Imgui.OpenGL;
import Lateralus.Platform.iPlatform;
import Lateralus.Platform.iWindow;
import Lateralus.Core;
import <atomic>;
import <mutex>;
import <optional>;
import <string_view>;

using namespace std;
using namespace std::string_view_literals;

namespace Lateralus::Platform::GLFW
{
    namespace 
    {
        constexpr uint32 k_OpenGLVersionMajor = 3;
        constexpr uint32 k_OpenGLVersionMinor = 2;
        char const* k_GlslVersion = "#version 150";
    }

    export
    class Window : public iWindow
    {
    public:
        ~Window() override
        {
            TryShutdown();
        }

        // iWindow
        optional<Error> Create(WindowCreateContext const& ctx) override
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
                GLFWmonitor* monitor = nullptr;
                // The window whose context to share resources with, or NULL to not share resources. 
                GLFWwindow* windowShare = nullptr;

                m_Window = glfwCreateWindow(width, height, title.c_str(), monitor, windowShare);
            }

            if (m_Window == nullptr)
            {
                return Error("Failed to create GLFWWindow");
            }

            glfwMakeContextCurrent(m_Window);
            // enable vsync
            glfwSwapInterval(1);

#if IMGUI_SUPPORT
            do
            {
                if (!IMGUI_CHECKVERSION())
                {
                    LOG_ERROR("Could not init imgui: IMGUI_CHECKVERSION()");
                    break;
                }

                m_ImguiContext = ImGui::CreateContext();
                if(m_ImguiContext == nullptr)
                {
                    LOG_ERROR("Could not init imgui: CreateContext()");
                    break;
                }

                ImGui::StyleColorsLight();

                if (!ImGui_ImplGlfw_InitForOpenGL(m_Window, true))
                {
                    LOG_ERROR("Could not init imgui: ImGui_ImplGlfw_InitForOpenGL({}, true)", reinterpret_cast<void*>(m_Window));
                    break;
                }

                if (!ImGui_ImplOpenGL3_Init(k_GlslVersion))
                {
                    LOG_ERROR("Could not init imgui: ImGui_ImplOpenGL3_Init(\"{}\")", k_GlslVersion);
                    break;
                }
            } while(false);
#endif


            if (GLenum result = glewInit(); result != GLEW_OK)
            {
                return Error(format("Couldn't init glew. Result: {}", result));
            }

            int32 screenWidth, screenHeight;
            glfwGetFramebufferSize(m_Window, &screenWidth, &screenHeight);
            glViewport(0, 0, static_cast<int>(screenWidth), static_cast<int>(screenHeight));
            
            return Success;
        }

        // iWindow
        bool ShouldClose() const override
        {
            return glfwWindowShouldClose(m_Window);
        }

        // iWindow
        void PollEvents() override
        {
            glfwPollEvents();
        }

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
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
#endif
        }

        // iWindow
        void Render() override
        {
#if IMGUI_SUPPORT
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
            if (m_ImguiContext != nullptr)
            {
                ImGui::SetCurrentContext(m_ImguiContext);
            }
#endif

            return Success;
        }

        optional<Error> TryGetFrameBufferSize(int32& outWidth, int32& outHeight)
        {
            if (m_Window == nullptr)
            {
                return Error("Unable to get frame buffer size: window missing.");
            }
            int framebufferWidth, framebufferHeight;
            glfwGetFramebufferSize(m_Window, &framebufferWidth, &framebufferHeight);
            outWidth =  static_cast<int32>(framebufferWidth);
            outHeight = static_cast<int32>(framebufferHeight);
            return Success;
        }

        optional<Error> TryShutdown()
        {
            optional<Error> problems = Success;

            if (m_Window == nullptr)
            {
                problems = Error("Unable to get frame buffer size: window missing.");
            }
            else
            {
                glfwDestroyWindow(m_Window);
                m_Window = nullptr;
            }

#if IMGUI_SUPPORT
            if (m_ImguiContext == nullptr)
            {
                problems = Error("Unable to shutdown imgui: imgui context missing.");
            }
            else
            {
                ImGui_ImplOpenGL3_Shutdown();
                ImGui::DestroyContext(m_ImguiContext);
                m_ImguiContext = nullptr;
            }
#endif
            return problems;
        }

        GLFWwindow* m_Window = nullptr;

#if IMGUI_SUPPORT
        ImGuiContext* m_ImguiContext = nullptr;
#endif
    };
}

#endif