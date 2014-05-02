
#pragma once

#include "../Renderer/3DRenderer/Mesh.h"
#include "../Renderer/3DRenderer/Texture.h"

struct GraphicalComponent
{
    Mesh mesh;
    Texture texture;

    void FreeComponent()
    {
        mesh.FreeVerticies();
        texture.FreeTexture();
    }
};
