
#include "PhysicsWorld.h"

PhysicsWorld::PhysicsWorld()
{
	m_CollisionCfg = new btDefaultCollisionConfiguration();
	m_Dispatcher = new btCollisionDispatcher(m_CollisionCfg);
	m_OverlappingPairCache = new btDbvtBroadphase();
	m_Solver = new btSequentialImpulseConstraintSolver();
	m_DynamicsWorld = new btDiscreteDynamicsWorld(
		m_Dispatcher, m_OverlappingPairCache, m_Solver, m_CollisionCfg);
}

PhysicsWorld::~PhysicsWorld()
{
	delete m_DynamicsWorld;
	delete m_Solver;
	delete m_OverlappingPairCache;
	delete m_Dispatcher;
	delete m_CollisionCfg;
}

btDiscreteDynamicsWorld *PhysicsWorld::GetDynamicsWorld() const
{
	return m_DynamicsWorld;
}
