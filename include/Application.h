#pragma once

int main(int argc, char** argv);

namespace PEANUT
{
    class Application
    {
        public:
        Application() = default;
        virtual void OnAttach() = 0;
        virtual void OnUpdate() = 0;
        virtual void OnRemove() = 0;
        bool m_shouldWindowClose;

        private:
        friend int ::main(int argc, char** argv);
        void Run();
    };

    /**
     * Implement this function on the client side to return the custom
     * application to run.
     * @return Application to run the engine with.
     */
    extern Application* GetApplication();
}