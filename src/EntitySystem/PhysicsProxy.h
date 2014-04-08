
#pragma once

#include "../Physics/DynamicsWorld.h"
#include "../Physics/MotionState.h"
#include "ComponentTable.h"
#include "PhysicalComponent.h"
#include "DynamicsComponent.h"

class PhysicsProxy
{
public:
    PhysicsProxy(DynamicsWorld* dynamics = nullptr):
        m_dynamics(dynamics),
        m_physical_components(nullptr),
        m_dynamics_components(nullptr)
    {
    }

    void SetComponentTables(ComponentTable<PhysicalComponent>* physical,
                            ComponentTable<DynamicsComponent>* dynamics)
    {
        m_physical_components = physical;
        m_dynamics_components = dynamics;
        CheckEntChanges();
    }

    void Cleanup()
    {
        CheckEntChanges();
    }

    void Simulate(uint32 dt)
    {
        assert(m_physical_components);
        assert(m_dynamics_components);

        CheckEntChanges();
        m_dynamics->Simulate(static_cast<float>(dt)/1000.0f);
        WriteBackSimulation();
    }
    
private:
    void CheckEntChanges()
    {
        auto *bullet_dynamics = m_dynamics->GetDynamicsWorld();

        auto &removals = m_dynamics_components->GetRemovals();
        for (uint ent : removals)
        {
            auto *component = m_dynamics_components->GetComponent(ent);
            auto *collision_obj = static_cast<btCollisionObject*>(component->shape->getUserPointer());
            bullet_dynamics->removeCollisionObject(collision_obj);

            auto *rigid_body = btRigidBody::upcast(collision_obj);
            if (rigid_body)
            {
                delete rigid_body->getMotionState();
                rigid_body->setMotionState(nullptr);
            }
            delete collision_obj;
            component->shape->setUserPointer(nullptr);
        }

        auto &additions = m_dynamics_components->GetAdditions();
        for (uint ent : additions)
        {
            assert(m_physical_components->HasComponent(ent));
            assert(m_dynamics_components->HasComponent(ent));

            auto *physical = m_physical_components->GetComponent(ent);
            auto *dynamics = m_dynamics_components->GetComponent(ent);

            auto *motionState = new MotionState(&m_moved_objects, physical->position, ent);
            btVector3 inertia(dynamics->inertia.x, dynamics->inertia.y, dynamics->inertia.z);

            dynamics->shape->calculateLocalInertia(dynamics->mass, inertia);
            auto *body = new btRigidBody(dynamics->mass, motionState, dynamics->shape, inertia);

            // Setting the user pointer to the collision object allows
            // accessing of the rigid body from the component.
            dynamics->shape->setUserPointer(static_cast<btCollisionObject*>(body));
            bullet_dynamics->addRigidBody(body);
        }
    }

    void WriteBackSimulation()
    {
        for (auto &obj : m_moved_objects)
        {
            auto *physical = m_physical_components->GetComponent(obj->GetEnt());
            physical->position = obj->GetPosition();
            physical->orientation = obj->GetOrientation();
        }
        m_moved_objects.clear();
    }

private:
    DynamicsWorld *m_dynamics;
    ComponentTable<PhysicalComponent> *m_physical_components;
    ComponentTable<DynamicsComponent> *m_dynamics_components;

    std::vector<MotionState*> m_moved_objects;
};
