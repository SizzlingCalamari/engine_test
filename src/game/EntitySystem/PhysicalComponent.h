
#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

struct PhysicalComponent
{
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    void FreeComponent()
    {
    }
};
