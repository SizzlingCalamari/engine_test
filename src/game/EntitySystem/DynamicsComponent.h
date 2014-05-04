
#pragma once

#include <glm/vec3.hpp>
#include "BulletCollision/CollisionShapes/btCollisionShape.h"

struct DynamicsComponent
{
    float mass = 0.0f;
    glm::vec3 inertia;
    btCollisionShape *shape = nullptr;

    void FreeComponent()
    {
        delete shape;
        shape = nullptr;
    }
};
