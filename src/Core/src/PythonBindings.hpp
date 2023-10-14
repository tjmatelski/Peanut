#include "Scene/Entity.h"
#include <Input/Input.h>
#include <Input/KeyCodes.h>

#include <filesystem>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

namespace PEANUT {
PYBIND11_EMBEDDED_MODULE(peanut, m)
{
    m.def("is_key_pressed", [](int i) {
        return PEANUT::Input::IsKeyPressed(static_cast<KeyCode>(i));
    });
    pybind11::enum_<KeyCode>(m, "KeyCode")
        .value("SPACE", KeyCode::SPACE)
        .value("APOSTROPHE", KeyCode::APOSTROPHE)
        .value("COMMA", KeyCode::COMMA)
        .value("MINUS", KeyCode::MINUS)
        .value("PERIOD", KeyCode::PERIOD)
        .value("SLASH", KeyCode::SLASH)
        .value("ZERO", KeyCode::ZERO)
        .value("ONE", KeyCode::ONE)
        .value("TWO", KeyCode::TWO)
        .value("THREE", KeyCode::THREE)
        .value("FOUR", KeyCode::FOUR)
        .value("FIVE", KeyCode::FIVE)
        .value("SIX", KeyCode::SIX)
        .value("SEVEN", KeyCode::SEVEN)
        .value("EIGHT", KeyCode::EIGHT)
        .value("NINE", KeyCode::NINE)
        .value("SEMICOLON", KeyCode::SEMICOLON)
        .value("EQUAL", KeyCode::EQUAL)
        .value("A", KeyCode::A)
        .value("B", KeyCode::B)
        .value("C", KeyCode::C)
        .value("D", KeyCode::D)
        .value("E", KeyCode::E)
        .value("F", KeyCode::F)
        .value("G", KeyCode::G)
        .value("H", KeyCode::H)
        .value("I", KeyCode::I)
        .value("J", KeyCode::J)
        .value("K", KeyCode::K)
        .value("L", KeyCode::L)
        .value("M", KeyCode::M)
        .value("N", KeyCode::N)
        .value("O", KeyCode::O)
        .value("P", KeyCode::P)
        .value("Q", KeyCode::Q)
        .value("R", KeyCode::R)
        .value("S", KeyCode::S)
        .value("T", KeyCode::T)
        .value("U", KeyCode::U)
        .value("V", KeyCode::V)
        .value("W", KeyCode::W)
        .value("X", KeyCode::X)
        .value("Y", KeyCode::Y)
        .value("Z", KeyCode::Z)
        .value("ESCAPE", KeyCode::ESCAPE)
        .value("ENTER", KeyCode::ENTER)
        .value("TAB", KeyCode::TAB)
        .value("BACKSPACE", KeyCode::BACKSPACE)
        .value("INSERT", KeyCode::INSERT)
        .value("DELETE", KeyCode::DELETE)
        .value("RIGHT", KeyCode::RIGHT)
        .value("LEFT", KeyCode::LEFT)
        .value("DOWN", KeyCode::DOWN)
        .value("UP", KeyCode::UP)
        .value("PAGE_UP", KeyCode::PAGE_UP)
        .value("PAGE_DOWN", KeyCode::PAGE_DOWN)
        .value("HOME", KeyCode::HOME)
        .value("END", KeyCode::END)
        .value("CAPS_LOCK", KeyCode::CAPS_LOCK)
        .value("SCROLL_LOCK", KeyCode::SCROLL_LOCK)
        .value("NUM_LOCK", KeyCode::NUM_LOCK)
        .value("PRINT_SCREEN", KeyCode::PRINT_SCREEN)
        .value("PAUSE", KeyCode::PAUSE)
        .value("F1", KeyCode::F1)
        .value("F2", KeyCode::F2)
        .value("F3", KeyCode::F3)
        .value("F4", KeyCode::F4)
        .value("F5", KeyCode::F5)
        .value("F6", KeyCode::F6)
        .value("F7", KeyCode::F7)
        .value("F8", KeyCode::F8)
        .value("F9", KeyCode::F9)
        .value("F10", KeyCode::F10)
        .value("F11", KeyCode::F11)
        .value("F12", KeyCode::F12)
        .value("F13", KeyCode::F13)
        .value("F14", KeyCode::F14)
        .value("F15", KeyCode::F15)
        .value("F16", KeyCode::F16)
        .value("F17", KeyCode::F17)
        .value("F18", KeyCode::F18)
        .value("F19", KeyCode::F19)
        .value("F20", KeyCode::F20)
        .value("F21", KeyCode::F21)
        .value("F22", KeyCode::F22)
        .value("F23", KeyCode::F23)
        .value("F24", KeyCode::F24)
        .value("F25", KeyCode::F25)
        .value("KP_0", KeyCode::KP_0)
        .value("KP_1", KeyCode::KP_1)
        .value("KP_2", KeyCode::KP_2)
        .value("KP_3", KeyCode::KP_3)
        .value("KP_4", KeyCode::KP_4)
        .value("KP_5", KeyCode::KP_5)
        .value("KP_6", KeyCode::KP_6)
        .value("KP_7", KeyCode::KP_7)
        .value("KP_8", KeyCode::KP_8)
        .value("KP_9", KeyCode::KP_9)
        .value("KP_DECIMAL", KeyCode::KP_DECIMAL)
        .value("KP_DIVIDE", KeyCode::KP_DIVIDE)
        .value("KP_MULTIPLY", KeyCode::KP_MULTIPLY)
        .value("KP_SUBTRACT", KeyCode::KP_SUBTRACT)
        .value("KP_ADD", KeyCode::KP_ADD)
        .value("KP_ENTER", KeyCode::KP_ENTER)
        .value("KP_EQUAL", KeyCode::KP_EQUAL)
        .value("LEFT_SHIFT", KeyCode::LEFT_SHIFT)
        .value("LEFT_CONTROL", KeyCode::LEFT_CONTROL)
        .value("LEFT_ALT", KeyCode::LEFT_ALT)
        .value("LEFT_SUPER", KeyCode::LEFT_SUPER)
        .value("RIGHT_SHIFT", KeyCode::RIGHT_SHIFT)
        .value("RIGHT_CONTROL", KeyCode::RIGHT_CONTROL)
        .value("RIGHT_ALT", KeyCode::RIGHT_ALT)
        .value("RIGHT_SUPER", KeyCode::RIGHT_SUPER)
        .value("MENU", KeyCode::MENU);
}
}