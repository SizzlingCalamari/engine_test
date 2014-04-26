
#pragma once

#include "ComponentTable.h"
#include "PhysicalComponent.h"
#include "DynamicsComponent.h"
#include <vector>

class DynamicsWorld;
class btCollisionObject;
class MotionState;

class PhysicsProxy
{
public:
    PhysicsProxy(DynamicsWorld* dynamics = nullptr);

    void SetComponentTables(ComponentTable<PhysicalComponent>* physical,
                            ComponentTable<DynamicsComponent>* dynamics);
    void Cleanup();

    void Simulate(uint32 dt);
    
private:
    void CheckEntChanges();

    void WriteBackSimulation();

    void RemoveCollisionObject(btCollisionObject *collision_obj);

private:
    DynamicsWorld *m_dynamics;
    ComponentTable<PhysicalComponent> *m_physical_components;
    ComponentTable<DynamicsComponent> *m_dynamics_components;

    std::vector<MotionState*> m_moved_objects;
};
