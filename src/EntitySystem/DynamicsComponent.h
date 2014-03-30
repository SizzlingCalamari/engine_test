
#pragma once

#include "btBulletDynamicsCommon.h"
#include <glm/glm.hpp>
#include <vector>

struct DynamicsComponent
{
    float mass; // = 0.0f;
    glm::vec3 inertia; // = { 0.0f, 0.0f, 0.0f };
    btCollisionShape *shape; // = nullptr;

    void FreeComponent()
    {
        delete shape;
        shape = nullptr;
    }
};
