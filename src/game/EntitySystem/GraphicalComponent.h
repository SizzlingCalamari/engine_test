
#pragma once

#include <string>

struct GraphicalComponent
{
    uint mesh = 0;
    uint material = 0;
    uint directionalLight = 0;
    uint pointLight = 0;
    uint spotLight = 0;

    void FreeComponent()
    {
    }
};
