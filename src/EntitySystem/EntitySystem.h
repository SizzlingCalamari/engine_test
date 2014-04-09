
#pragma once

#include "ComponentTable.h"
#include "PhysicalComponent.h"
#include "GraphicalComponent.h"
#include "DynamicsComponent.h"

class EntitySystem
{
public:
    EntitySystem():
        m_ent_counter(0)
    {
    }

    uint CreateEntity()
    {
        return m_ent_counter++;
    }

    void DestroyEntity(uint ent)
    {
        m_physical_components.RemoveComponent(ent);
        m_graphical_components.RemoveComponent(ent);
        m_dynamics_components.RemoveComponent(ent);
    }

    void CommitChanges()
    {
        m_physical_components.CommitChanges();
        m_graphical_components.CommitChanges();
        m_dynamics_components.CommitChanges();
    }

    //
    // Attaching components
    //
    void AttachComponent(uint ent, PhysicalComponent* component)
    {
        return m_physical_components.AttachComponent(ent, component);
    }

    void AttachComponent(uint ent, GraphicalComponent* component)
    {
        return m_graphical_components.AttachComponent(ent, component);
    }

    void AttachComponent(uint ent, DynamicsComponent* component)
    {
        return m_dynamics_components.AttachComponent(ent, component);
    }

    //
    // Removing components
    //
    template<typename T>
    void RemoveComponent(uint ent);

    //
    // Getting component tables
    //
    template<typename T>
    ComponentTable<T>* GetTable();

private:
    ComponentTable<PhysicalComponent> m_physical_components;
    ComponentTable<GraphicalComponent> m_graphical_components;
    ComponentTable<DynamicsComponent> m_dynamics_components;

    uint m_ent_counter;
};

template<>
inline void EntitySystem::RemoveComponent<PhysicalComponent>(uint ent)
{
    m_physical_components.RemoveComponent(ent);
}

template<>
inline void EntitySystem::RemoveComponent<GraphicalComponent>(uint ent)
{
    m_graphical_components.RemoveComponent(ent);
}

template<>
inline ComponentTable<PhysicalComponent>* EntitySystem::GetTable()
{
    return &m_physical_components;
}

template<>
inline ComponentTable<GraphicalComponent>* EntitySystem::GetTable()
{
    return &m_graphical_components;
}

template<>
inline ComponentTable<DynamicsComponent>* EntitySystem::GetTable()
{
    return &m_dynamics_components;
}
