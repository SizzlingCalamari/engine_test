
#pragma once

#include "btBulletDynamicsCommon.h"

class PhysicsWorld
{
public:
	PhysicsWorld();
	~PhysicsWorld();

	btDiscreteDynamicsWorld *GetDynamicsWorld() const;

private:
	btDefaultCollisionConfiguration *m_CollisionCfg;
	btCollisionDispatcher *m_Dispatcher;
	btBroadphaseInterface *m_OverlappingPairCache;
	btSequentialImpulseConstraintSolver *m_Solver;
	btDiscreteDynamicsWorld *m_DynamicsWorld;
};
