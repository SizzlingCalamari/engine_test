
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
        auto *object = static_cast<btCollisionObject*>(shape->getUserPointer());
        assert(object);

        auto *rigid_body = btRigidBody::upcast(object);
        if (rigid_body)
        {
            delete rigid_body->getMotionState();
        }
        delete object;
        delete shape;
    }
};
