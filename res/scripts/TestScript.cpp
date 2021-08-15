#include <Scene/NativeScript.h>
#include <Input.h>
#include <Scene/Component.h>

#include <iostream>

namespace PEANUT
{

class TestScript : public NativeScript
{
public:
    float speed = 10.0f;
    virtual void OnUpdate(TimeStep ts) override
    {
        std::cout << "Updating Test Script" << std::endl;
        if (Input::IsKeyPressed(KeyCode::SPACE))
        {
            auto& position = GetComponent<TransformComponent>().translation;
            position.y += speed * ts;
        }
    }
};

}

extern "C" PEANUT::NativeScript* GetScript()
{
    return new PEANUT::TestScript();
}