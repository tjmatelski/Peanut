#pragma once

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
    [[nodiscard]] auto GetMembers() const -> const std::vector<MemberVariable>& { return m_members; }

protected:
    std::vector<MemberVariable> m_members;
};
}