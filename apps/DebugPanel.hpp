#pragma once

#include "Panel.hpp"

namespace PEANUT {
class Engine;

class DebugPanel : public Panel {
public:
    DebugPanel(class Engine* engine)
        : Panel("Debug", engine)
    {
    }

    void Update() override;

private:
};
}