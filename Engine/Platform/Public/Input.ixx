module;

#include <Core.h>

export module Lateralus.Platform.Input;

import <array>;
import <functional>;
import <optional>;

import Lateralus.Core;
import Lateralus.Core.Signal;
import Lateralus.Platform.Error;

using namespace std;

namespace Lateralus::Platform
{
    class iWindow;
}

// Input values are designed to map exactly to GLFW input values for an easy first implementation.
namespace Lateralus::Platform::Input
{
    export
    enum class KeyModifier
    {
        Shift       = 0b00000001,
        Control     = 0b00000010,
        Alt         = 0b00000100,
        Super       = 0b00001000,
        CapsLock    = 0b00010000,
        NumLock     = 0b00100000
    };

    ENUM_FLAG_OPERATORS(KeyModifier);

    export
    enum class KeyAction
    {
        Release,
        Press,
        Repeat
    };

    export
    enum class KeyCode
    {
        Key_UNKNOWN = -1,

        Key_SPACE = 32,
        Key_APOSTROPHE = 39, // '
        Key_COMMA = 44, // ,
        Key_MINUS = 45, // -
        Key_PERIOD = 46, // .
        Key_SLASH = 47, // /
        Key_0 = 48,
        Key_1 = 49,
        Key_2 = 50,
        Key_3 = 51,
        Key_4 = 52,
        Key_5 = 53,
        Key_6 = 54,
        Key_7 = 55,
        Key_8 = 56,
        Key_9 = 57,
        Key_SEMICOLON = 59, // ;
        Key_EQUAL = 61, // =
        Key_A = 65,
        Key_B = 66,
        Key_C = 67,
        Key_D = 68,
        Key_E = 69,
        Key_F = 70,
        Key_G = 71,
        Key_H = 72,
        Key_I = 73,
        Key_J = 74,
        Key_K = 75,
        Key_L = 76,
        Key_M = 77,
        Key_N = 78,
        Key_O = 79,
        Key_P = 80,
        Key_Q = 81,
        Key_R = 82,
        Key_S = 83,
        Key_T = 84,
        Key_U = 85,
        Key_V = 86,
        Key_W = 87,
        Key_X = 88,
        Key_Y = 89,
        Key_Z = 90,
        Key_LEFT_BRACKET = 91, // [
        Key_BACKSLASH = 92, /* \ */
        Key_RIGHT_BRACKET = 93, // ]
        Key_GRAVE_ACCENT = 96, // `
        Key_WORLD_1 = 161, // non-US #1
        Key_WORLD_2 = 162, // non-US #2


        Key_ESCAPE = 256,
        Key_ENTER = 257,
        Key_TAB = 258,
        Key_BACKSPACE = 259,
        Key_INSERT = 260,
        Key_DELETE = 261,
        Key_RIGHT = 262,
        Key_LEFT = 263,
        Key_DOWN = 264,
        Key_UP = 265,
        Key_PAGE_UP = 266,
        Key_PAGE_DOWN = 267,
        Key_HOME = 268,
        Key_END = 269,
        Key_CAPS_LOCK = 280,
        Key_SCROLL_LOCK = 281,
        Key_NUM_LOCK = 282,
        Key_PRINT_SCREEN = 283,
        Key_PAUSE = 284,
        Key_F1 = 290,
        Key_F2 = 291,
        Key_F3 = 292,
        Key_F4 = 293,
        Key_F5 = 294,
        Key_F6 = 295,
        Key_F7 = 296,
        Key_F8 = 297,
        Key_F9 = 298,
        Key_F10 = 299,
        Key_F11 = 300,
        Key_F12 = 301,
        Key_F13 = 302,
        Key_F14 = 303,
        Key_F15 = 304,
        Key_F16 = 305,
        Key_F17 = 306,
        Key_F18 = 307,
        Key_F19 = 308,
        Key_F20 = 309,
        Key_F21 = 310,
        Key_F22 = 311,
        Key_F23 = 312,
        Key_F24 = 313,
        Key_F25 = 314,
        Key_KP_0 = 320,
        Key_KP_1 = 321,
        Key_KP_2 = 322,
        Key_KP_3 = 323,
        Key_KP_4 = 324,
        Key_KP_5 = 325,
        Key_KP_6 = 326,
        Key_KP_7 = 327,
        Key_KP_8 = 328,
        Key_KP_9 = 329,
        Key_KP_DECIMAL = 330,
        Key_KP_DIVIDE = 331,
        Key_KP_MULTIPLY = 332,
        Key_KP_SUBTRACT = 333,
        Key_KP_ADD = 334,
        Key_KP_ENTER = 335,
        Key_KP_EQUAL = 336,
        Key_LEFT_SHIFT = 340,
        Key_LEFT_CONTROL = 341,
        Key_LEFT_ALT = 342,
        Key_LEFT_SUPER = 343,
        Key_RIGHT_SHIFT = 344,
        Key_RIGHT_CONTROL = 345,
        Key_RIGHT_ALT = 346,
        Key_RIGHT_SUPER = 347,
        Key_MENU = 348,

        Key_LAST = Key_MENU
    };

    export
    constexpr
    bool KeyCodeIsUnknown(KeyCode code)
    {
        return code == KeyCode::Key_UNKNOWN;
    }

    export
    constexpr
    bool KeyCodeIsPrintable(KeyCode code)
    {
        return code >= KeyCode::Key_SPACE && code <= KeyCode::Key_WORLD_2;
    }

    export
    constexpr
    bool KeyCodeIsFunction(KeyCode code)
    {
        return code >= KeyCode::Key_ESCAPE && code <= KeyCode::Key_MENU;
    }

    export
    enum class MouseButton
    {
        MouseButton_1 = 0,
        MouseButton_2,
        MouseButton_3,

        MouseButton_4,
        MouseButton_5,
        MouseButton_6,
        MouseButton_7,
        MouseButton_8,

        COUNT,
        
        Left = MouseButton_1,
        Right = MouseButton_2,
        Middle = MouseButton_3,
    };

    export
    enum class MouseButtonAction
    {
        Release,
        Press,
        Repeat
    };

    // Text callback one unicode character at a time (utf-8; characters may be multiple bytes).
    export
    using TextCallback = Signal<void(u8string_view), SignalLockMutex>;
    // Key action callback one key at a time (press, release, etc).
    export
    using KeyActionCallback = Signal<void(KeyCode, KeyAction, KeyModifier), SignalLockMutex>;

    // On mouse move reports the current cursor position (x, y)
    export
    using CursorPositionCallback = Signal<void(double, double), SignalLockMutex>;
    // (Desktop) cursor entered the window
    export
    using CursorEnterCallback = Signal<void(iWindow*), SignalLockMutex>;
    // (Desktop) cursor exited the window
    export
    using CursorLeaveCallback = Signal<void(iWindow*), SignalLockMutex>;
    // (Desktop) Mouse button action (click down, release, etc).
    export
    using MouseButtonCallback = Signal<void(MouseButton, MouseButtonAction, KeyModifier), SignalLockMutex>;
    // (Desktop) Scroll wheel delta
    export
    using ScrollWheelCallback = Signal<void(double, double), SignalLockMutex>;

    export
    class iInputProvider
    {
    public:
        iInputProvider() = default;
        virtual ~iInputProvider() = default;

        virtual optional<Error> Init() = 0;
        virtual void Shutdown() = 0;

        // Keyboard / Text
        ENCAPSULATE_SIGNAL(TextCallback, TextCallback);
        ENCAPSULATE_SIGNAL(KeyActionCallback, KeyActionCallback);

        // Mouse / Touch
        ENCAPSULATE_SIGNAL(CursorPositionCallback, CursorPositionCallback);
        ENCAPSULATE_SIGNAL(CursorEnterCallback, CursorEnterCallback);
        ENCAPSULATE_SIGNAL(CursorLeaveCallback, CursorLeaveCallback);
        ENCAPSULATE_SIGNAL(MouseButtonCallback, MouseButtonCallback);
        ENCAPSULATE_SIGNAL(ScrollWheelCallback, ScrollWheelCallback);
    };
}
