
#pragma once

#include <glm/vec3.hpp>

struct CylindricalBillboard
{
    glm::vec3 initialDirection = glm::vec3(0.0f, 1.0f, 0.0f);
    float maxAngleFromForward = 0.0f;
};
