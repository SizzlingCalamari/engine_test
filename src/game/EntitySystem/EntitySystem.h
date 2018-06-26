
#pragma once

#include "ComponentTable.h"

struct PhysicalComponent;
struct GraphicalComponent;
struct DynamicsComponent;

class EntitySystem
{
public:
    EntitySystem();
    ~EntitySystem();

    uint CreateEntity()
    {
        return ++m_ent_counter;
    }

    void DestroyEntity(uint ent);

    void CommitChanges();

    //
    // Attaching components
    //
    template<typename T>
    void AttachComponent(uint ent, T* component)
    {
        GetTable<T>()->AttachComponent(ent, component);
    }

    //
    // Removing components
    //
    template<typename T>
    void RemoveComponent(uint ent)
    {
        GetTable<T>()->RemoveComponent(ent);
    }

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
