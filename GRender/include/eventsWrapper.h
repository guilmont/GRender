#pragma once

#include "imgui.h"

namespace GRender {

enum class MouseButton : int32_t {
    LEFT = 0,
    RIGHT = 1,
    MIDDLE = 2
    };

enum class Key : int32_t {
    NONE = ImGuiKey_None,
    SPACE = ImGuiKey_Space,
    APOSTROPHE = ImGuiKey_Apostrophe,
    COMMA = ImGuiKey_Comma,
    MINUS = ImGuiKey_Minus,
    PERIOD = ImGuiKey_Period,
    SLASH = ImGuiKey_Slash,
    SEMICOLON = ImGuiKey_Semicolon,
    EQUAL = ImGuiKey_Equal,
    LEFT_BRACKET = ImGuiKey_LeftBracket,
    BACKSLASH = ImGuiKey_Backslash,
    RIGHT_BRACKET = ImGuiKey_RightBracket,
    GRAVE_ACCENT = ImGuiKey_GraveAccent,
    ESCAPE = ImGuiKey_Escape,

    ENTER = ImGuiKey_Enter,
    TAB = ImGuiKey_Tab,
    BACKSPACE = ImGuiKey_Backspace,
    INSERT = ImGuiKey_Insert,
    DELETE = ImGuiKey_Delete,
    RIGHT = ImGuiKey_RightArrow,
    LEFT = ImGuiKey_LeftArrow,
    DOWN = ImGuiKey_DownArrow,
    UP = ImGuiKey_UpArrow,
    PAGE_UP = ImGuiKey_PageUp,
    PAGE_DOWN = ImGuiKey_PageDown,
    HOME = ImGuiKey_Home,
    END = ImGuiKey_End,
    CAPS_LOCK = ImGuiKey_CapsLock,
    SCROLL_LOCK = ImGuiKey_ScrollLock,
    NUM_LOCK = ImGuiKey_NumLock,
    PRINT_SCREEN = ImGuiKey_PrintScreen,
    PAUSE = ImGuiKey_Pause,

    F1 = ImGuiKey_F1,
    F2 = ImGuiKey_F2,
    F3 = ImGuiKey_F3,
    F4 = ImGuiKey_F4,
    F5 = ImGuiKey_F5,
    F6 = ImGuiKey_F6,
    F7 = ImGuiKey_F7,
    F8 = ImGuiKey_F8,
    F9 = ImGuiKey_F9,
    F10 = ImGuiKey_F10,
    F11 = ImGuiKey_F11,
    F12 = ImGuiKey_F12,

    KP_0 = ImGuiKey_Keypad0,
    KP_1 = ImGuiKey_Keypad1,
    KP_2 = ImGuiKey_Keypad2,
    KP_3 = ImGuiKey_Keypad3,
    KP_4 = ImGuiKey_Keypad4,
    KP_5 = ImGuiKey_Keypad5,
    KP_6 = ImGuiKey_Keypad6,
    KP_7 = ImGuiKey_Keypad7,
    KP_8 = ImGuiKey_Keypad8,
    KP_9 = ImGuiKey_Keypad9,

    KP_DECIMAL = ImGuiKey_KeypadDecimal,
    KP_DIVIDE = ImGuiKey_KeypadDivide,
    KP_MULTIPLY = ImGuiKey_KeypadMultiply,
    KP_SUBTRACT = ImGuiKey_KeypadSubtract,
    KP_ADD = ImGuiKey_KeypadAdd,
    KP_ENTER = ImGuiKey_KeypadEnter,
    KP_EQUAL = ImGuiKey_KeypadEqual,
    LEFT_SHIFT = ImGuiKey_LeftShift,
    LEFT_CONTROL = ImGuiKey_LeftCtrl,
    LEFT_SUPER = ImGuiKey_LeftSuper,
    RIGHT_SHIFT = ImGuiKey_RightShift,
    RIGHT_CONTROL = ImGuiKey_RightCtrl,
    RIGHT_ALT = ImGuiKey_RightAlt,
    RIGHT_SUPER = ImGuiKey_RightSuper,
};

} // namespace GRender