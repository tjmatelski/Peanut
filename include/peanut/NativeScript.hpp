#pragma once

#include <peanut/Entity.hpp>

// stl
#include <string>
#include <vector>

namespace PEANUT {

struct MemberVariable {
    enum class Type {
        Bool,
        Int,
        Float,
        Double,
        String
    };

    std::string name;
    Type type;
    void* addr;
};

class NativeScript {
public:
    NativeScript();
    virtual ~NativeScript();
    NativeScript(const NativeScript&);
    NativeScript(NativeScript&&);

    virtual void OnCreate() { }
    virtual void OnDestroy() { }
    virtual void EditorUpdate() { }
    virtual void RuntimeBegin() { }
    virtual void Update([[maybe_unused]] double dt) { }
    virtual void RuntimeEnd() { }

    [[nodiscard]] auto GetMembers() const -> const std::vector<MemberVariable>& { return m_members; }

protected:
    std::vector<MemberVariable> m_members;
    Entity m_ent;
    friend Entity;
};
}