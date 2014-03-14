
#include "Renderer.h"
#include "3DRenderer/3DRenderer.h"

namespace Renderer
{
    Renderer3D *CreateAndInit3DRenderer(void *GLContext)
    {
        assert(GLContext);

        // glew needs to be initialized after 
        // the gl_createcontext call
        static RunOnce glew([]
        {
            glewExperimental = true;
            glewInit();
        });

        auto renderer = new Renderer3D(GLContext);
        renderer->Init();
        return renderer;
    }

    void FreeRenderer(Renderer3D *r)
    {
        assert(r);
        delete r;
    }
}
