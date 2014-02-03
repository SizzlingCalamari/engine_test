
#pragma once

#include "btBulletDynamicsCommon.h"

typedef btCollisionObject PhysicsComponentInternal;
class PhysicsComponentSystem;

struct PhysicsComponentInfo
{
	btScalar mass = 0.0f;
	btVector3 inertia = { 0.0f, 0.0f, 0.0f };
	btTransform transform = btTransform::getIdentity();
	btCollisionShape *shape = nullptr;
};

class PhysicsComponent
{
	friend class PhysicsComponentSystem;
public:
	void SetInfo(const PhysicsComponentInfo &info);

protected:
	PhysicsComponent(PhysicsComponentSystem *sys, uint32 ent);

private:
	PhysicsComponentSystem *m_PhysicsSystem;
	uint32 m_ent;
};
