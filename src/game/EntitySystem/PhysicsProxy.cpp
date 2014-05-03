
#include "PhysicsProxy.h"

#include "../Physics/DynamicsWorld.h"
#include "../Physics/MotionState.h"

#include "ComponentTable.h"
#include "PhysicalComponent.h"
#include "DynamicsComponent.h"

void PhysicsProxy::Cleanup()
{
    auto *bullet_dynamics = m_dynamics->GetDynamicsWorld();

    auto &collision_objects = bullet_dynamics->getCollisionObjectArray();
    auto num_obj = collision_objects.size();
    for (auto i = 0; i < num_obj; ++i)
    {
        RemoveCollisionObject(collision_objects[i]);
    }
}

void PhysicsProxy::Simulate(uint32 dt)
{
    assert(m_physical_components);
    assert(m_dynamics_components);

    CheckEntChanges();
    m_dynamics->Simulate(static_cast<float>(dt)/1000.0f);
    WriteBackSimulation();
}

void PhysicsProxy::CheckEntChanges()
{
    auto *bullet_dynamics = m_dynamics->GetDynamicsWorld();

    auto &removals = m_dynamics_components->GetRemovals();
    for (uint ent : removals)
    {
        auto *component = m_dynamics_components->PeekComponent(ent);
        auto *collision_obj = static_cast<btCollisionObject*>(component->shape->getUserPointer());
        RemoveCollisionObject(collision_obj);
    }

    auto &additions = m_dynamics_components->GetAdditions();
    for (uint ent : additions)
    {
        assert(m_physical_components->HasComponent(ent));
        assert(m_dynamics_components->HasComponent(ent));

        auto *physical = m_physical_components->PeekComponent(ent);
        auto *dynamics = m_dynamics_components->PeekComponent(ent);

        auto *motionState = new MotionState(&m_moved_objects, physical->position,
                                            physical->orientation, ent);
        btVector3 inertia(dynamics->inertia.x, dynamics->inertia.y, dynamics->inertia.z);

        dynamics->shape->calculateLocalInertia(dynamics->mass, inertia);
        auto *body = new btRigidBody(dynamics->mass, motionState, dynamics->shape, inertia);

        // Setting the user pointer to the shape allows accessing
        // accessing of the shape from the collision object.
        body->setUserPointer(dynamics->shape);

        // Setting the user pointer to the collision object allows
        // accessing of the rigid body from the component.
        dynamics->shape->setUserPointer(static_cast<btCollisionObject*>(body));
        bullet_dynamics->addRigidBody(body);
    }
}

void PhysicsProxy::WriteBackSimulation()
{
    for (auto &obj : m_moved_objects)
    {
        const uint ent = obj->GetEnt();
        auto physical = m_physical_components->GetComponent(ent);
        physical.position = obj->GetPosition();
        physical.orientation = obj->GetOrientation();
        m_physical_components->EditComponent(ent, &physical);
    }
    m_moved_objects.clear();
}

void PhysicsProxy::RemoveCollisionObject(btCollisionObject *collision_obj)
{
    m_dynamics->GetDynamicsWorld()->removeCollisionObject(collision_obj);

    auto *rigid_body = btRigidBody::upcast(collision_obj);
    if (rigid_body)
    {
        delete rigid_body->getMotionState();
        rigid_body->setMotionState(nullptr);
    }
    auto *shape = static_cast<btCollisionShape*>(collision_obj->getUserPointer());
    shape->setUserPointer(nullptr);
    delete collision_obj;
}
