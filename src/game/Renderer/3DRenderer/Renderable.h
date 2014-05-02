
#pragma once

#include "Mesh.h"
#include "Texture.h"
#include <glm/glm.hpp>

struct Renderable
{
    glm::mat4 transform;
    Mesh mesh;
    Texture texture;
};
