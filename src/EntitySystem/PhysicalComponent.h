
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct PhysicalComponent
{
    glm::vec3 position;
    glm::quat orientation;

    void FreeComponent()
    {
    }
};
