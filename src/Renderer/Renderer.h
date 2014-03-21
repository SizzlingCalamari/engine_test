
#pragma once

class Renderer3D;

namespace Renderer
{
    Renderer3D *CreateRenderer3D(void *GLContext);

    void FreeRenderer(Renderer3D *r);
}
