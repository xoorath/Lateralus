module;

#include <GL/glew.h>
// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

#include "Platform.Log.h"

export module Lateralus.Platform.Windows;


import <optional>;
import <format>;
import <string_view>;

import Lateralus.Core;
import Lateralus.Platform.Error;

using namespace std;
using namespace std::string_view_literals;

namespace Lateralus::Platform::Windows
{
    namespace Internal
    {
        void GlfwErrorCallback(int error, const char* description)
        {
            PLATFORM_LOG("GLFW Error {}: {}", error, description);
        }
    }

    export std::optional<Error> Init()
    {
        glfwSetErrorCallback(Internal::GlfwErrorCallback);
        if (!glfwInit())
            return Error("GLFW failed to initialize.");

        constexpr uint32 OpenGLVersionMajor = 3;
        constexpr uint32 OpenGLVersionMinor = 2;
        constexpr std::string_view GlslVersion = "#version 150"sv;
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OpenGLVersionMajor);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OpenGLVersionMinor);

        return {};
    }
}