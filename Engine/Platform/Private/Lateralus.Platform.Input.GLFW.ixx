module;

#include <GLFW/glfw3.h>

export module Lateralus.Platform.Input.GLFW;

#if USE_GLFW_WINDOW

import Lateralus.Core;
import Lateralus.Core.UtfConversion;
import Lateralus.Platform.Error;
import Lateralus.Platform.Input;
import <array>;
import <optional>;

using namespace std;
using namespace Lateralus::Core;

namespace Lateralus::Platform::Input::GLFW
{
export class InputProvider : public iInputProvider
{
public:
    InputProvider() = default;
    virtual ~InputProvider() { Shutdown(); }

    optional<Error> Init(GLFWwindow *window)
    {
        if (m_Window != nullptr)
        {
            return Error("Multiple initialization of glfw input provider.");
        }
        m_Window = window;
        return Init();
    }

    void Shutdown() override
    {
        if (m_Window != nullptr)
        {
            glfwSetKeyCallback(m_Window, nullptr);
            glfwSetCharCallback(m_Window, nullptr);
            glfwSetMouseButtonCallback(m_Window, nullptr);
            glfwSetScrollCallback(m_Window, nullptr);
            glfwSetWindowUserPointer(m_Window, nullptr);
            m_Window = nullptr;
        }
    }

private:
    optional<Error> Init() override
    {
        if (m_Window == nullptr)
        {
            return Error("GLFW Input missing glfw window.");
        }

        if (glfwGetWindowUserPointer(m_Window) != nullptr)
        {
            return Error("GLFW Input overwriting glfw window user pointer.");
        }

        glfwSetWindowUserPointer(m_Window, reinterpret_cast<void *>(this));

        glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode, int action,
                                        int mods) {
            KeyCode const keyCode = static_cast<KeyCode>(key);
            KeyAction const keyAction = static_cast<KeyAction>(action);
            KeyModifier const keyModifier = static_cast<KeyModifier>(mods);

            auto self = reinterpret_cast<InputProvider const *>(glfwGetWindowUserPointer(window));
            self->m_KeyActionCallback(keyCode, keyAction, keyModifier);
        });

        glfwSetCharCallback(m_Window, [](GLFWwindow *window, unsigned int codepoint) {
            usz size;
            auto utfCharacter = UTF32_to_UTF8(codepoint, size);
            u8string_view utfStrView(utfCharacter.data(), size);

            auto self = reinterpret_cast<InputProvider const *>(glfwGetWindowUserPointer(window));
            self->m_TextCallback(utfStrView);
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button, int action,
                                                int mods) {
            MouseButton const mouseButton = static_cast<MouseButton>(button);
            MouseButtonAction const mouseAction = static_cast<MouseButtonAction>(action);
            KeyModifier const keyMods = static_cast<KeyModifier>(action);

            auto self = reinterpret_cast<InputProvider const *>(glfwGetWindowUserPointer(window));
            self->m_MouseButtonCallback(mouseButton, mouseAction, keyMods);
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double x, double y) {
            auto self = reinterpret_cast<InputProvider const *>(glfwGetWindowUserPointer(window));
            self->m_ScrollWheelCallback(x, y);
        });
        return Success;
    }

    GLFWwindow *m_Window = nullptr;
};
} // namespace Lateralus::Platform::Input::GLFW
#endif