
#pragma once

#include <glm/glm.hpp>

struct PhysicalComponent
{
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 up;

    void FreeComponent()
    {
    }
};
