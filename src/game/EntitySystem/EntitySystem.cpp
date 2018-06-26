
#include "EntitySystem.h"

#include "PhysicalComponent.h"
#include "GraphicalComponent.h"
#include "DynamicsComponent.h"

EntitySystem::EntitySystem():
    m_ent_counter(0)
{
}

EntitySystem::~EntitySystem()
{
}

void EntitySystem::DestroyEntity(uint ent)
{
    m_physical_components.RemoveComponent(ent);
    m_graphical_components.RemoveComponent(ent);
    m_dynamics_components.RemoveComponent(ent);
}

void EntitySystem::CommitChanges()
{
    m_physical_components.CommitChanges();
    m_graphical_components.CommitChanges();
    m_dynamics_components.CommitChanges();
}

template<>
ComponentTable<PhysicalComponent>* EntitySystem::GetTable()
{
    return &m_physical_components;
}

template<>
ComponentTable<GraphicalComponent>* EntitySystem::GetTable()
{
    return &m_graphical_components;
}

template<>
ComponentTable<DynamicsComponent>* EntitySystem::GetTable()
{
    return &m_dynamics_components;
}
