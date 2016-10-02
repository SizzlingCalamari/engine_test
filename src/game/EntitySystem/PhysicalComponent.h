
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct PhysicalComponent
{
    glm::vec3 position;
    glm::quat orientation;
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    void FreeComponent()
    {
    }
};
