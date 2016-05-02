
#include "DynamicsWorld.h"

DynamicsWorld::DynamicsWorld()
{
    m_BulletContext.collision_config = new btDefaultCollisionConfiguration();
    m_BulletContext.dispatcher = new btCollisionDispatcher(
        m_BulletContext.collision_config);
    m_BulletContext.overlapping_pair_cache = new btDbvtBroadphase();
    m_BulletContext.solver = new btSequentialImpulseConstraintSolver();
	m_DynamicsWorld = new btDiscreteDynamicsWorld(
        m_BulletContext.dispatcher,
        m_BulletContext.overlapping_pair_cache,
        m_BulletContext.solver,
        m_BulletContext.collision_config);
}

DynamicsWorld::~DynamicsWorld()
{
	delete m_DynamicsWorld;
    delete m_BulletContext.solver;
    delete m_BulletContext.overlapping_pair_cache;
    delete m_BulletContext.dispatcher;
    delete m_BulletContext.collision_config;
}

void DynamicsWorld::Initialize()
{
    m_DynamicsWorld->setGravity({ 0.0f, -9.8f, 0.0f });
}

void DynamicsWorld::Shutdown()
{
    auto objects = m_DynamicsWorld->getCollisionObjectArray();
    auto num_objects = m_DynamicsWorld->getNumCollisionObjects();
    for (auto i = 0; i < num_objects; ++i)
    {
        m_DynamicsWorld->removeCollisionObject(objects[i]);
    }
}

void DynamicsWorld::Simulate(float dt)
{
    m_DynamicsWorld->stepSimulation(dt);
}
