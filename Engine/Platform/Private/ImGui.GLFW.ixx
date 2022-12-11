module;
#if IMGUI_SUPPORT && USE_GLFW_WINDOW
// License from: https://github.com/ocornut/imgui/blob/master/LICENSE.txt
// Source code from: https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_glfw.cpp
/*
The MIT License (MIT)

Copyright (c) 2014-2022 Omar Cornut

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// Modifications by Jared Thomson in 2022 are mostly stylistic to fit within the
// Lateralus project. Modifications include:
// * C++20 module support (removal of header, exporting public functions)
// * Pragma disable warning 5105 from Winbase.h
// * Moving functionality and state into a class (ImplGLFW)
// * Callbacks are now handled with iInputProvide
// * Definition of __SPECSTRINGS_STRICT_LEVEL to suppress macro redefinition
//   from specstrings_strict.h (windows sdk)

#include <GLFW/glfw3.h>
#if PLATFORM_WIN64
// A lot of this code is just to cope with a macro redefinition bug:
// https://developercommunity.visualstudio.com/t/warning-C4005:-Outptr:-macro-redefinit/1546919 Each
// define or undefine below marked with [hack] should be removed when the above is fixed.
#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0 // [#hack]
#define __SPECSTRINGS_STRICT_LEVEL 0                                         // [#hack]
#undef APIENTRY
#include <windows.h>
#undef __nullnullterminated // [#hack]
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif
#include "freetype/imgui_freetype.h"
#include "imgui.h"
#endif
export module Lateralus.Platform.Imgui.GLFW;
#if IMGUI_SUPPORT
import <array>;
import <memory>;
import <optional>;
import Lateralus.Core;
import Lateralus.Platform.Imgui.iImpl;
import Lateralus.Platform.Error;
import Lateralus.Platform.Input;
import Lateralus.Platform.Font.NotoEmojiRegular;
import Lateralus.Platform.Font.NotoSansRegular;

using namespace std;
using namespace Lateralus::Platform::Input;
using namespace Lateralus::Platform::Font;

namespace Lateralus::Platform::Imgui
{
export class ImplGLFW : public iImpl
{
public:
    ImplGLFW() = default;

    virtual ~ImplGLFW() { Shutdown(); }

    optional<Error> Init(GLFWwindow *window, shared_ptr<Input::iInputProvider> input)
    {
        if (m_Window != nullptr || m_Input != nullptr)
        {
            return Error("Multiple initialization of impl glfw.");
        }
        m_Window = window;
        m_Input = move(input);

        return Init();
    }

    void Shutdown()
    {
        for (auto const cursor : m_MouseCursors)
        {
            glfwDestroyCursor(cursor);
        }
        m_MouseCursors.fill(nullptr);

        if (m_Window != nullptr)
        {
            m_Window = nullptr;
        }

        if (m_Input != nullptr)
        {
            m_Input->GetTextCallback() -= m_TextCallbackToken;
            m_Input->GetKeyActionCallback() -= m_KeyActionCallbackToken;
            m_Input->GetMouseButtonCallback() -= m_MouseButtonCallbackToken;
            m_Input->GetScrollWheelCallback() -= m_ScrollWheelCallbackToken;

            m_Input.reset();
        }
    }

    void NewFrame()
    {
        ImGuiIO &io = ImGui::GetIO();
        IM_ASSERT(io.Fonts->IsBuilt() &&
                  "Font atlas not built! It is generally built by the renderer back-end. Missing "
                  "call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

        // Setup display size (every frame to accommodate for window resizing)
        int w, h;
        int display_w, display_h;
        glfwGetWindowSize(m_Window, &w, &h);
        glfwGetFramebufferSize(m_Window, &display_w, &display_h);
        io.DisplaySize = ImVec2((float)w, (float)h);
        if (w > 0 && h > 0)
        {
            io.DisplayFramebufferScale = ImVec2((float)display_w / w, (float)display_h / h);
        }

        // Setup time step
        double current_time = glfwGetTime();
        io.DeltaTime = m_Time > 0.0 ? (float)(current_time - m_Time) : (float)(1.0f / 60.0f);
        m_Time = current_time;

        UpdateMousePosAndButtons();
        UpdateMouseCursor();

        // Update game controllers (if enabled and available)
        UpdateGamepads();
    }

    void LoadFonts()
    {
        ImGuiIO &io = ImGui::GetIO();

        {
            static ImFontConfig cfg;
            strcpy_s(cfg.Name, "Noto Sans Regular");
            io.Fonts->AddFontFromMemoryCompressedTTF(reinterpret_cast<void *>(NotoSansRegular_data),
                                                     static_cast<int>(NotoSansRegular_size), 24,
                                                     &cfg, io.Fonts->GetGlyphRangesDefault());
        }

        {
            static ImWchar ranges[] = {(ImWchar)0x1, (ImWchar)0x1FFFF, (ImWchar)0};
            static ImFontConfig cfg;
            strcpy_s(cfg.Name, "Noto Emoji Regular");
            cfg.OversampleH = cfg.OversampleV = 1;
            cfg.MergeMode = true;
            cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;

            io.Fonts->AddFontFromMemoryCompressedTTF(
                reinterpret_cast<void *>(NotoEmojiRegular_data),
                static_cast<int>(NotoEmojiRegular_size), 24, &cfg, ranges);
        }
    }

private:
    optional<Error> Init() override
    {
        if (m_Window == nullptr)
        {
            return Error("glfw window missing");
        }

        if (m_Input == nullptr)
        {
            return Error("platform input missing");
        }

        m_Time = 0.0;

        // Setup back-end capabilities flags
        ImGuiIO &io = ImGui::GetIO();
        io.BackendFlags |=
            ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; // We can honor io.WantSetMousePos
                                                             // requests (optional, rarely used)
        io.BackendPlatformName = "Lateralus::Platform::Imgui::GLFWImpl";

        // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
        io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
        io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
        io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
        io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
        io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
        io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
        io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
        io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
        io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
        io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
        io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
        io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
        io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
        io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
        io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

        io.SetClipboardTextFn = ImplGLFW::SetClipboardText;
        io.GetClipboardTextFn = ImplGLFW::GetClipboardText;

        io.ClipboardUserData = m_Window;

#if PLATFORM_WIN64
        io.ImeWindowHandle = (void *)glfwGetWin32Window(m_Window);
#endif

        m_MouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        m_MouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
        m_MouseCursors[ImGuiMouseCursor_ResizeAll] =
            glfwCreateStandardCursor(GLFW_ARROW_CURSOR); // FIXME: GLFW doesn't have this.
        m_MouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
        m_MouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
        m_MouseCursors[ImGuiMouseCursor_ResizeNESW] =
            glfwCreateStandardCursor(GLFW_ARROW_CURSOR); // FIXME: GLFW doesn't have this.
        m_MouseCursors[ImGuiMouseCursor_ResizeNWSE] =
            glfwCreateStandardCursor(GLFW_ARROW_CURSOR); // FIXME: GLFW doesn't have this.
        m_MouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

        // Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks,
        // if any.

        if (m_Input != nullptr)
        {
            m_TextCallbackToken = m_Input->GetTextCallback() += [this](u8string_view input) {
                array<char8_t, 5> buff = {'\0'};
                copy(input.begin(), input.end(), // from
                     buff.begin()                // to
                );
                ImGuiIO &io = ImGui::GetIO();
                io.AddInputCharactersUTF8(reinterpret_cast<char const *>(buff.data()));
            };

            m_KeyActionCallbackToken = m_Input->GetKeyActionCallback() +=
                [this](KeyCode code, KeyAction action, KeyModifier modifier) {
                    ImGuiIO &io = ImGui::GetIO();
                    if (action == KeyAction::Press)
                    {
                        io.KeysDown[(int)code] = true;
                    }
                    if (action == KeyAction::Release)
                    {
                        io.KeysDown[(int)code] = false;
                    }

                    // Modifiers are not reliable across systems
                    io.KeyCtrl = io.KeysDown[(int)KeyCode::Key_LEFT_CONTROL] ||
                                 io.KeysDown[(int)KeyCode::Key_RIGHT_CONTROL];
                    io.KeyShift = io.KeysDown[(int)KeyCode::Key_LEFT_SHIFT] ||
                                  io.KeysDown[(int)KeyCode::Key_RIGHT_SHIFT];
                    io.KeyAlt = io.KeysDown[(int)KeyCode::Key_LEFT_ALT] ||
                                io.KeysDown[(int)KeyCode::Key_RIGHT_ALT];
                    io.KeySuper = io.KeysDown[(int)KeyCode::Key_LEFT_SUPER] ||
                                  io.KeysDown[(int)KeyCode::Key_RIGHT_SUPER];
                };

            m_MouseButtonCallbackToken = m_Input->GetMouseButtonCallback() +=
                [this](MouseButton button, MouseButtonAction action, KeyModifier modifiers) {
                    if (action == MouseButtonAction::Press)
                    {
                        m_MouseJustPressed[static_cast<size_t>(button)] = true;
                    }
                };

            m_ScrollWheelCallbackToken = m_Input->GetScrollWheelCallback() +=
                [](double x, double y) {
                    ImGuiIO &io = ImGui::GetIO();
                    io.MouseWheelH += static_cast<float>(x);
                    io.MouseWheel += static_cast<float>(y);
                };
        }

        LoadFonts();

        return Success;
    }

    void UpdateMousePosAndButtons()
    {
        // Update buttons
        ImGuiIO &io = ImGui::GetIO();
        for (usz i = 0; i < m_MouseJustPressed.size(); i++)
        {
            // If a mouse press event came, always pass it as "mouse held this frame", so we don't
            // miss click-release events that are shorter than 1 frame.
            io.MouseDown[i] = m_MouseJustPressed[i] || glfwGetMouseButton(m_Window, i) != 0;
            m_MouseJustPressed[i] = false;
        }

        // Update mouse position
        const ImVec2 mouse_pos_backup = io.MousePos;
        io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

        if (bool const focused = glfwGetWindowAttrib(m_Window, GLFW_FOCUSED) != 0; focused)
        {
            if (io.WantSetMousePos)
            {
                glfwSetCursorPos(m_Window, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y);
            }
            else
            {
                double mouse_x, mouse_y;
                glfwGetCursorPos(m_Window, &mouse_x, &mouse_y);
                io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
            }
        }
    }

    void UpdateMouseCursor()
    {
        ImGuiIO &io = ImGui::GetIO();
        if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) ||
            glfwGetInputMode(m_Window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        {
            return;
        }

        ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
        if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
        {
            // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
        else
        {
            // Show OS mouse cursor
            // FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with
            // GLFW 3.2, but 3.3 works here.
            glfwSetCursor(m_Window, m_MouseCursors[imgui_cursor]
                                        ? m_MouseCursors[imgui_cursor]
                                        : m_MouseCursors[ImGuiMouseCursor_Arrow]);
            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    void UpdateGamepads()
    {
        ImGuiIO &io = ImGui::GetIO();
        memset(io.NavInputs, 0, sizeof(io.NavInputs));
        if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
        {
            return;
        }

        // Update gamepad inputs
#define MAP_BUTTON(NAV_NO, BUTTON_NO)                                                              \
    {                                                                                              \
        if (buttons_count > BUTTON_NO && buttons[BUTTON_NO] == GLFW_PRESS)                         \
            io.NavInputs[NAV_NO] = 1.0f;                                                           \
    }
#define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1)                                                        \
    {                                                                                              \
        float v = (axes_count > AXIS_NO) ? axes[AXIS_NO] : V0;                                     \
        v = (v - V0) / (V1 - V0);                                                                  \
        if (v > 1.0f)                                                                              \
            v = 1.0f;                                                                              \
        if (io.NavInputs[NAV_NO] < v)                                                              \
            io.NavInputs[NAV_NO] = v;                                                              \
    }
        int axes_count = 0, buttons_count = 0;
        const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
        unsigned char const *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);
        MAP_BUTTON(ImGuiNavInput_Activate, 0);   // Cross / A
        MAP_BUTTON(ImGuiNavInput_Cancel, 1);     // Circle / B
        MAP_BUTTON(ImGuiNavInput_Menu, 2);       // Square / X
        MAP_BUTTON(ImGuiNavInput_Input, 3);      // Triangle / Y
        MAP_BUTTON(ImGuiNavInput_DpadLeft, 13);  // D-Pad Left
        MAP_BUTTON(ImGuiNavInput_DpadRight, 11); // D-Pad Right
        MAP_BUTTON(ImGuiNavInput_DpadUp, 10);    // D-Pad Up
        MAP_BUTTON(ImGuiNavInput_DpadDown, 12);  // D-Pad Down
        MAP_BUTTON(ImGuiNavInput_FocusPrev, 4);  // L1 / LB
        MAP_BUTTON(ImGuiNavInput_FocusNext, 5);  // R1 / RB
        MAP_BUTTON(ImGuiNavInput_TweakSlow, 4);  // L1 / LB
        MAP_BUTTON(ImGuiNavInput_TweakFast, 5);  // R1 / RB
        MAP_ANALOG(ImGuiNavInput_LStickLeft, 0, -0.3f, -0.9f);
        MAP_ANALOG(ImGuiNavInput_LStickRight, 0, +0.3f, +0.9f);
        MAP_ANALOG(ImGuiNavInput_LStickUp, 1, +0.3f, +0.9f);
        MAP_ANALOG(ImGuiNavInput_LStickDown, 1, -0.3f, -0.9f);
#undef MAP_BUTTON
#undef MAP_ANALOG
        if (axes_count > 0 && buttons_count > 0)
        {
            io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
        }
        else
        {
            io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
        }
    }

    static char const *GetClipboardText(void *user_data)
    {
        return glfwGetClipboardString((GLFWwindow *)user_data);
    }

    static void SetClipboardText(void *user_data, const char *text)
    {
        glfwSetClipboardString((GLFWwindow *)user_data, text);
    }

    GLFWwindow *m_Window = nullptr;
    double m_Time = 0.0;
    array<bool, (int)MouseButton::COUNT> m_MouseJustPressed = {false};
    array<GLFWcursor *, ImGuiMouseCursor_COUNT> m_MouseCursors = {nullptr};

    TextCallback::Token m_TextCallbackToken;
    KeyActionCallback::Token m_KeyActionCallbackToken;
    MouseButtonCallback::Token m_MouseButtonCallbackToken;
    ScrollWheelCallback::Token m_ScrollWheelCallbackToken;

    shared_ptr<Input::iInputProvider> m_Input;
};
} // namespace Lateralus::Platform::Imgui
#endif