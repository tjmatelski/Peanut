#include <Input.h>
#include <Scene/Component.h>
#include <Scene/NativeScript.h>

#include <iostream>

namespace PEANUT {

class TestScript : public NativeScript {
public:
    float speed = 10.0f;
    virtual void OnUpdate(TimeStep ts) override
    {
        std::cout << "Updating Test Script" << std::endl;
        if (Input::IsKeyPressed(KeyCode::SPACE)) {
            auto& position = GetComponent<TransformComponent>().translation;
            position.y += speed * ts;
        }
    }
};

}

extern "C" __attribute__((visibility("default"))) PEANUT::NativeScript* GetScript()
{
    return new PEANUT::TestScript();
}
