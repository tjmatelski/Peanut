#include "Engine.hpp"
#include "Input/Input.h"
#include "Input/KeyCodes.h"
#include "Scene/Component.h"
#include "Scene/Entity.h"
#include "Utils/Log.h"

#include <glm/fwd.hpp>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <filesystem>
#include <string>
#include <unordered_map>
#include <variant>

PYBIND11_MAKE_OPAQUE(PEANUT::EditorFieldMap);

namespace py = pybind11;

namespace PEANUT {

struct PythonScript {
    PythonScript() { LOG_DEBUG("Constructing PythonScript: {:p}", fmt::ptr(this)); }
    virtual ~PythonScript() { LOG_DEBUG("Destructing PythonScript: {:p}", fmt::ptr(this)); }

    // Component access
    Entity m_ent;
    template <class CompT>
    CompT& get() { return m_ent.Get<CompT>(); }
    template <class CompT>
    void set(const CompT& comp) { m_ent.Get<CompT>() = comp; }

    // Editor
    EditorFieldMap editor_fields;

    virtual void runtime_begin() { }
    virtual void update(double dt) { }
    virtual void runtime_end() { }
    virtual void editor_update() { }
};

struct PythonScriptBinding : public PythonScript {
    PythonScriptBinding()
        : PythonScript()
    {
        LOG_DEBUG("Constructing PythonScriptBinding: {:p}", fmt::ptr(this));
    }

    PythonScriptBinding(PythonScript&& base)
        : PythonScript(std::move(base))
    {
        LOG_DEBUG("Constructing PythonScriptBinding: {:p}", fmt::ptr(this));
    }

    ~PythonScriptBinding() override
    {
        LOG_DEBUG("Destructing PythonScriptBinding: {:p}", fmt::ptr(this));
    }

    void runtime_begin() override
    {
        PYBIND11_OVERRIDE(void, PythonScript, runtime_begin);
    }

    void update(double dt) override
    {
        PYBIND11_OVERRIDE(void, PythonScript, update, dt);
    }

    void runtime_end() override
    {
        PYBIND11_OVERRIDE(void, PythonScript, runtime_end);
    }

    void editor_update() override
    {
        PYBIND11_OVERRIDE(void, PythonScript, editor_update);
    }
};

PYBIND11_EMBEDDED_MODULE(peanut, m)
{
    py::bind_map<EditorFieldMap>(m, "EditorFieldsMap");

    py::class_<EditorButton>(m, "EditorButton")
        .def(py::init())
        .def_readwrite("pressed", &EditorButton::pressed);

    // Components
    py::class_<glm::vec3>(m, "Vec3")
        .def(py::init())
        .def_readwrite("x", &glm::vec3::x)
        .def_readwrite("y", &glm::vec3::y)
        .def_readwrite("z", &glm::vec3::z);

    py::class_<TransformComponent>(m, "TransformComponent")
        .def(py::init())
        .def_readwrite("translation", &TransformComponent::translation)
        .def_readwrite("rotation", &TransformComponent::rotation)
        .def_readwrite("scale", &TransformComponent::scale);

    py::class_<PythonScript, PythonScriptBinding>(m, "PythonScript")
        .def(py::init<>())
        .def("runtime_begin", &PythonScript::runtime_begin)
        .def("update", &PythonScript::update)
        .def("runtime_end", &PythonScript::runtime_end)
        .def("editor_update", &PythonScript::editor_update)
        .def_property("transform", &PythonScript::get<TransformComponent>, &PythonScript::set<TransformComponent>, py::return_value_policy::reference)
        .def_readwrite("editor_fields", &PythonScript::editor_fields, py::return_value_policy::reference);

    m.def("is_key_pressed", [](int i) {
        return PEANUT::Input::IsKeyPressed(static_cast<KeyCode>(i));
    });

    py::enum_<KeyCode>(m, "KeyCode")
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
