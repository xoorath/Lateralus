module;

#include <GL/glew.h>
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <Core.Log.h>

export module Lateralus.Platform.Windows.Platform;

import <format>;
import <mutex>;
import <optional>;
import <string_view>;

import Lateralus.Core;
import Lateralus.Platform.Error;
import Lateralus.Platform.iPlatform;

using namespace std;
using namespace std::string_view_literals;

namespace Lateralus::Platform::Windows
{
    namespace
    {
        void GlfwErrorCallback(int error, const char* description)
        {
            LOG_ERROR("GLFW Error {}: {}", error, description);
        }
    }

    export 
    class Platform : public iPlatform
    {
    public:
        optional<Error> Init() override
        {
            lock_guard guard(m_Mutex);
            if (m_Initialized)
            {
                return {};
            }

            glfwSetErrorCallback(GlfwErrorCallback);
            if (!glfwInit())
                return Error("GLFW failed to initialize.");

            m_Initialized = true;
            return Success;
        }

    private:
        mutex m_Mutex;
        bool m_Initialized = false;
    };

}
