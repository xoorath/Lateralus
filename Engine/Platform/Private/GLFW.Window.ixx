module;

#if PLATFORM_WIN64

#include <GL/glew.h>
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <Core.Log.h>

export module Lateralus.Platform.GLFW.Window;

import Lateralus.Platform.Error;
import Lateralus.Platform.iPlatform;
import Lateralus.Platform.iWindow;
import Lateralus.Core;
import <optional>;
import <string_view>;

using namespace std;
using namespace std::string_view_literals;

namespace Lateralus::Platform::GLFW
{
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

            if (auto err = TryMakeContextCurrent(); err.has_value())
            {
                return err;
            }

            glfwSwapInterval(1);

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
        void SwapBuffers() override
        {
            int32 screenWidth, screenHeight;
            glfwGetFramebufferSize(m_Window, &screenWidth, &screenHeight);
            glViewport(0, 0, static_cast<int>(screenWidth), static_cast<int>(screenHeight));

            glfwSwapBuffers(m_Window);
        }

        // iWindow
        void* HACK_GetGlfwWindow() override
        { 
            return reinterpret_cast<void*>(m_Window);
        }

        string GetGlslVersion() override
        {
            return k_GlslVersion;
        }

    private:
        optional<Error> TryMakeContextCurrent()
        {
            if (m_Window == nullptr)
            {
                return Error("Unable to make window current: window missing.");
            }
            glfwMakeContextCurrent(m_Window);
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
            if (m_Window == nullptr)
            {
                return Error("Unable to get frame buffer size: window missing.");
            }
            glfwDestroyWindow(m_Window);
            glfwTerminate();
            m_Window = nullptr;
            return Success;
        }

        GLFWwindow* m_Window = nullptr;

        static constexpr uint32 k_OpenGLVersionMajor = 3;
        static constexpr uint32 k_OpenGLVersionMinor = 2;
        static constexpr std::string k_GlslVersion = "#version 150";
    };
}

#endif