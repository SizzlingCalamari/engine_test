
#pragma once

struct GraphicalComponent
{
    GraphicalComponent():
        mesh(0),
        material(0),
        directionalLight(0),
        pointLight(0),
        spotLight(0)
    {
    }
    uint mesh;
    uint material;
    uint directionalLight;
    uint pointLight;
    uint spotLight;

    void FreeComponent()
    {
    }
};
