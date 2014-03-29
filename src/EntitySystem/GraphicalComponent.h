
#pragma once

#include "../Renderer/3DRenderer/Mesh.h"

struct GraphicalComponent
{
    Mesh mesh;

    void FreeComponent()
    {
        mesh.FreeVerticies();
    }
};
