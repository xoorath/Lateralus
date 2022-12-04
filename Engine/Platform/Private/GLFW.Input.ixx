module;

#include <Core.Log.h>
#include <GLFW/glfw3.h>

export module Lateralus.Platform.GLFW.Input;

import Lateralus.Core;
import Lateralus.Core.UtfConversion;
import Lateralus.Platform.Error;
import Lateralus.Platform.Input;
import <array>;
import <format>;
import <optional>;
import <unordered_map>;

using namespace std;

namespace Lateralus::Platform::Input::GLFW
{
    export
    class InputProvider : public iInputProvider
    {
    public:
        InputProvider() = default;
        virtual ~InputProvider()
        {
            Shutdown();
        }

        optional<Error> Init(GLFWwindow* window)
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
                if (auto found = s_InputProviders.find(m_Window); found != s_InputProviders.end())
                {
                    s_InputProviders.erase(found);
                }

                m_Window = nullptr;
            }
        }

    private:
        optional<Error> Init() override
        {
            if (m_Window == nullptr)
            {
                return Error("Glfw input missing glfw window.");
            }

            s_InputProviders[m_Window] = this;

            glfwSetKeyCallback(m_Window, KeyCallback);
            glfwSetCharCallback(m_Window, TextCallback);
            glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);
            glfwSetScrollCallback(m_Window, ScrollWheelCallback);

            return Success;
        }

        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            if (auto found = s_InputProviders.find(window); found != s_InputProviders.end())
            {
                found->second->_KeyCallback(window, key, scancode, action, mods);
            }
        }

        void _KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            KeyCode keyCode = static_cast<KeyCode>(key);
            KeyAction keyAction = static_cast<KeyAction>(action);
            KeyModifier keyModifier = static_cast<KeyModifier>(mods);

            m_KeyActionCallback(keyCode, keyAction, keyModifier);
        }

        static void TextCallback(GLFWwindow* window, unsigned int codepoint)
        {
            if (auto found = s_InputProviders.find(window); found != s_InputProviders.end())
            {
                found->second->_TextCallback(window, codepoint);
            }
        }

        void _TextCallback(GLFWwindow* window, unsigned int codepoint)
        {
            usz size;
            auto utfCharacter = UTF32_to_UTF8(codepoint, size);
            u8string_view utfStrView(utfCharacter.data(), size);
            m_TextCallback(utfStrView);
        }

        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
        {
            if (auto found = s_InputProviders.find(window); found != s_InputProviders.end())
            {
                found->second->_MouseButtonCallback(window, button, action, mods);
            }
        }

        void _MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
        {
            m_MouseButtonCallback(static_cast<MouseButton>(button), static_cast<MouseButtonAction>(action), static_cast<KeyModifier>(mods));
        }

        static void ScrollWheelCallback(GLFWwindow* window, double x, double y)
        {
            if (auto found = s_InputProviders.find(window); found != s_InputProviders.end())
            {
                found->second->_ScrollWheelCallback(window, x, y);
            }
        }

        void _ScrollWheelCallback(GLFWwindow* window, double x, double y)
        {
            m_ScrollWheelCallback(x, y);
        }

        GLFWwindow* m_Window = nullptr;
        static unordered_map<GLFWwindow*, InputProvider*> s_InputProviders;
    };

    unordered_map<GLFWwindow*, InputProvider*> InputProvider::s_InputProviders;
}