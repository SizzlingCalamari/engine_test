
#include "Renderer.h"
#include "3DRenderer/3DRenderer.h"
#include "utils.h"

namespace Renderer
{
    Renderer3D *CreateRenderer3D(void *GLContext)
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
        return renderer;
    }

    void FreeRenderer(Renderer3D *r)
    {
        assert(r);
        delete r;
    }
}
