
#include "DynamicsComponent.h"

#include "BulletCollision/CollisionShapes/btCollisionShape.h"

DynamicsComponent::DynamicsComponent():
    mass(0.0f),
    shape(nullptr)
{
}

DynamicsComponent::~DynamicsComponent() = default;

void DynamicsComponent::FreeComponent()
{
    delete shape;
    shape = nullptr;
}

