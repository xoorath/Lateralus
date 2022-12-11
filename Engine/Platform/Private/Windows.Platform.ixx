module;

#include <Core.Log.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions

export module Lateralus.Platform.Windows.Platform;

import <format>;
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
void GlfwErrorCallback(int error, const char *description)
{
    LOG_ERROR("GLFW Error {}: {}", error, description);
}
} // namespace

export class Platform : public iPlatform
{
public:
    optional<Error> Init() override
    {
        glfwSetErrorCallback(GlfwErrorCallback);
        if (glfwInit() != GLFW_TRUE)
        {
            const char *description = nullptr;
            int code = glfwGetError(&description);
            if (description != nullptr)
            {
                return Error(format("Could not init glfw. Error({}) : {}", code, description));
            }
            return Error(format("Could not init glfw. Error code: {}", code));
        }

        return Success;
    }

    ~Platform() override { glfwTerminate(); }
};

} // namespace Lateralus::Platform::Windows
