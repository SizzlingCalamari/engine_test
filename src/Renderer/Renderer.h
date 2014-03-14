
#pragma once

class Renderer3D;

namespace Renderer
{
    Renderer3D *CreateAndInit3DRenderer(void *GLContext);

    void FreeRenderer(Renderer3D *r);
}
