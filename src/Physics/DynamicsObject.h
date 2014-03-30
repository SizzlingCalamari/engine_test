
#pragma once

#include "btBulletDynamicsCommon.h"

struct DynamicsComponent
{
    btScalar mass; // = 0.0f;
    btVector3 inertia; // = { 0.0f, 0.0f, 0.0f };
    btTransform transform; // = btTransform::getIdentity();
    btCollisionShape *shape; // = nullptr;
};
