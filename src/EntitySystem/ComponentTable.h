
#pragma once

#include <tuple>
#include <iostream>

template<typename T>
class ComponentTable
{
public:
    T* AttachComponent(uint ent)
    {
        auto it = m_ent_to_component.find(ent);
        if (it == m_ent_to_component.end())
        {
            uint index = m_components.size();

            // add the component
            m_components.emplace_back();

            // map the ent to an array index
            m_ent_to_component.emplace(ent, index);
            m_component_to_ent.emplace(index, ent);

            return m_components.data() + index;
        }
        return nullptr;
    }

    void RemoveComponent(uint ent)
    {
        auto it = m_ent_to_component.find(ent);
        if (it != m_ent_to_component.end())
        {
            uint index = it.second;
            uint last_elem = m_components.size()-1;
            
            // swap last and removed element
            m_components[index] = m_components[last_elem];

            // fix map indices
            m_ent_to_component[last_elem] = m_ent_to_component[index];
            m_component_to_ent[index] = m_component_to_ent[last_elem];

            // remove element from map
            m_ent_to_component.erase(it);
        }
    }

    T* GetComponent(uint ent)
    {
        return m_components.data() + m_ent_to_component[ent];
    }

    std::pair<const T*, uint> GetComponentArray() const
    {
        return std::make_pair(m_components.data(), m_components.size());
    }

private:
    std::unordered_map<uint, uint> m_ent_to_component;
    std::unordered_map<uint, uint> m_component_to_ent;
    std::vector<T> m_components;
};
