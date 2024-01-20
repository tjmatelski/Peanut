#pragma once

#include "Scene/Entity.h"

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

    virtual void Update([[maybe_unused]] double dt) { }

    [[nodiscard]] auto GetMembers() const -> const std::vector<MemberVariable>& { return m_members; }

protected:
    std::vector<MemberVariable> m_members;
    Entity m_ent;
    friend Entity;
};
}