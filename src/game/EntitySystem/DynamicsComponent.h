
#pragma once

#include <glm/vec3.hpp>

class btCollisionShape;

struct DynamicsComponent
{
    DynamicsComponent();
    ~DynamicsComponent();

    void FreeComponent();

    float mass;
    glm::vec3 inertia;
    btCollisionShape *shape;
};
