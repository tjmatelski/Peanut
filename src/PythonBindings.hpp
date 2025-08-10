// peanut
#include <peanut/Engine.hpp>
#include <peanut/Entity.hpp>

// external

// stl

namespace PEANUT {

class PythonScript {
public:
    PythonScript();
    virtual ~PythonScript();

    // Component access
    Entity m_ent;
    template <class CompT> CompT& get() { return m_ent.Get<CompT>(); }
    template <class CompT> void set(const CompT& comp) { m_ent.Get<CompT>() = comp; }

    // Editor
    Engine::EditorFieldMap editor_fields;

    virtual void runtime_begin();
    virtual void update(double /* dt */);
    virtual void runtime_end();
    virtual void editor_update();
};

struct PythonScriptBinding : public PythonScript {
    PythonScriptBinding();
    PythonScriptBinding(PythonScript&& base);
    ~PythonScriptBinding() override;

    void runtime_begin() override;
    void update(double dt) override;
    void runtime_end() override;
    void editor_update() override;
};

}
