
#pragma once

#include "ComponentTable.h"
#include "PhysicalComponent.h"
#include "GraphicalComponent.h"
#include "DynamicsComponent.h"

class EntitySystem
{
public:
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

    template<>
    void RemoveComponent<PhysicalComponent>(uint ent)
    {
        m_physical_components.RemoveComponent(ent);
    }

    template<>
    void RemoveComponent<GraphicalComponent>(uint ent)
    {
        m_graphical_components.RemoveComponent(ent);
    }

    //
    // Getting component tables
    //
    template<typename T>
    ComponentTable<T>* GetTable();

    template<>
    ComponentTable<PhysicalComponent>* GetTable()
    {
        return &m_physical_components;
    }

    template<>
    ComponentTable<GraphicalComponent>* GetTable()
    {
        return &m_graphical_components;
    }

    template<>
    ComponentTable<DynamicsComponent>* GetTable()
    {
        return &m_dynamics_components;
    }

private:
    ComponentTable<PhysicalComponent> m_physical_components;
    ComponentTable<GraphicalComponent> m_graphical_components;
    ComponentTable<DynamicsComponent> m_dynamics_components;
};
