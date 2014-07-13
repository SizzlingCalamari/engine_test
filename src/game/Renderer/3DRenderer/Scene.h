
#pragma once

#include "Mesh.h"
#include "Texture.h"
#include <glm/mat4x4.hpp>

struct SceneNode
{
    glm::mat4 transform;
    const Mesh *mesh;
    const Texture *texture;
};
