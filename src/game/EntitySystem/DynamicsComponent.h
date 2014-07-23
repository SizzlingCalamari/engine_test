
#pragma once

#include <glm/vec3.hpp>
#include "BulletCollision/CollisionShapes/btCollisionShape.h"

struct DynamicsComponent
{
    DynamicsComponent():
        mass(0.0f),
        shape(nullptr)
    {
    }

    float mass;
    glm::vec3 inertia;
    btCollisionShape *shape;

    void FreeComponent()
    {
        delete shape;
        shape = nullptr;
    }
};
