
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
            auto index = m_components.size();

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
            uint removed_index = it->second;
            uint tail_index = m_components.size()-1;
            uint tail_ent = m_component_to_ent[tail_index];
            if (removed_index != tail_index)
            {
                // swap last and removed element
                m_components[removed_index] = std::move(m_components[tail_index]);

                // fix map indices
                m_ent_to_component[tail_ent] = removed_index;
                m_component_to_ent[removed_index] = tail_ent;

                // remove element from map
                m_ent_to_component.erase(it);
                m_component_to_ent.erase(tail_index);
            }
            else
            {
                // erase mapping from removed ent
                m_ent_to_component.erase(it);
                m_component_to_ent.erase(removed_index);
            }
            m_components.pop_back();
        }
    }

    bool HasComponent(uint ent)
    {
        return (m_ent_to_component.find(ent) != m_ent_to_component.end());
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
    std::unordered_map<size_t, size_t> m_ent_to_component;
    std::unordered_map<size_t, size_t> m_component_to_ent;
    std::vector<T> m_components;
};
