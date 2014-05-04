
#pragma once

#include <glm/mat4x4.hpp>

struct Renderable
{
    glm::mat4 transform;
    uint mesh = 0;
    uint texture = 0;
};
