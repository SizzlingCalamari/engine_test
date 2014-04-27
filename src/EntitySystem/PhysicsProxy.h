
#pragma once

#include <vector>

class DynamicsWorld;
class btCollisionObject;
class MotionState;

template<typename T> class ComponentTable;
struct PhysicalComponent;
struct DynamicsComponent;

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

inline PhysicsProxy::PhysicsProxy(DynamicsWorld* dynamics /*= nullptr*/):
    m_dynamics(dynamics),
    m_physical_components(nullptr),
    m_dynamics_components(nullptr)
{
}

inline void PhysicsProxy::SetComponentTables(ComponentTable<PhysicalComponent>* physical,
                                             ComponentTable<DynamicsComponent>* dynamics)
{
    m_physical_components = physical;
    m_dynamics_components = dynamics;
    CheckEntChanges();
}
