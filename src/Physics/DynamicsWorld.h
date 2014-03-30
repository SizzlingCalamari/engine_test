
#pragma once

#include "btBulletDynamicsCommon.h"

struct BulletPhysicsContext
{
    btDefaultCollisionConfiguration *collision_config;
    btCollisionDispatcher *dispatcher;
    btBroadphaseInterface *overlapping_pair_cache;
    btSequentialImpulseConstraintSolver *solver;
};

class DynamicsWorld
{
public:
    DynamicsWorld();
    ~DynamicsWorld();

    void Initialize();
    void Shutdown();

    void Simulate(float dt);

    btDiscreteDynamicsWorld* GetDynamicsWorld() const
    {
        return m_DynamicsWorld;
    }

private:
    BulletPhysicsContext m_BulletContext;
	btDiscreteDynamicsWorld *m_DynamicsWorld;
};
