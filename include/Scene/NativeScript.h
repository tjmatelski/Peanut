#pragma once

#include "Entity.h"
#include <TimeStep.h>

namespace PEANUT {

struct NativeScript {
    virtual ~NativeScript() { }
    virtual void OnUpdate([[maybe_unused]] TimeStep ts) { }
    template <typename T>
    T& GetComponent()
    {
        return m_entity.Get<T>();
    }
    Entity m_entity;
};

} // namespace PEANUT
