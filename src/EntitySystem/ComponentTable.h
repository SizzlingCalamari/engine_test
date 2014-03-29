
#pragma once

#include <tuple>
#include <iostream>

template<typename T>
class ComponentTable
{
public:
    void AttachComponent(uint ent, T* component)
    {
        auto it = m_ent_to_component.find(ent);
        if (it == m_ent_to_component.end())
        {
            m_additions.emplace_back(std::make_pair(ent, std::move(*component)));
        }
    }

    void RemoveComponent(uint ent)
    {
        auto it = m_ent_to_component.find(ent);
        if (it != m_ent_to_component.end())
        {
            auto *component = m_components.data() + it->second;
            m_removals.emplace_back(std::make_pair(ent, component));
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

    const std::vector<T>& GetComponentArray() const
    {
        return m_components;
    }

    const std::vector<std::pair<uint, T>>& GetAdditions() const
    {
        return m_additions;
    }

    const std::vector<std::pair<uint, T*>>& GetRemovals() const
    {
        return m_removals;
    }

    void CommitChanges()
    {
        auto less_compare = [](std::pair<size_t, size_t> a, std::pair<size_t, size_t> b)
        {
            return a.first < b.first;
        };
       
        auto equals_compare = [](std::pair<size_t, size_t> a, std::pair<size_t, size_t> b)
        {
            return a.first == b.first;
        };


        // Get removed component indices.
        std::vector<std::pair<size_t, size_t>> remove_pairs;
        remove_pairs.reserve(m_removals.size());
        {
            for (auto &pair : m_removals)
            {
                auto ent = pair.first;
                auto it = m_ent_to_component.find(ent);
                remove_pairs.emplace_back(it->second, 0);
                m_ent_to_component.erase(it);
            }
        }
        // Sort by removal index
        std::sort(remove_pairs.begin(), remove_pairs.end(), less_compare);

        // Get replacement component indices.
        {
            auto i = m_components.size() - 1;
            for (auto &pair : remove_pairs)
            {
                if (i <= pair.first)
                {
                    break;
                }

                bool result = std::binary_search(remove_pairs.begin(), remove_pairs.end(), std::make_pair(i, i), less_compare);
                if (!result)
                {
                    pair.second = i;
                }
                --i;
            }
        }

        // Perform replacements
        {
            for (auto &pair : remove_pairs)
            {
                auto removing_index = pair.first;
                auto replacement_index = pair.second;
                auto replacement_ent = m_component_to_ent[pair.second];
                auto &component = m_components[removing_index];

                component.FreeComponent();
                component = std::move(m_components[replacement_index]);
                m_ent_to_component[replacement_ent] = removing_index;
                m_component_to_ent[removing_index] = replacement_ent;
                m_component_to_ent.erase(replacement_index);
            }

            for (auto &pair : remove_pairs)
            {
                m_components.pop_back();
            }
        }
        m_removals.clear();

        // Perform additions
        for (auto &pair : m_additions)
        {
            auto ent = pair.first;
            auto index = m_components.size();

            // add the component
            m_components.emplace_back(std::move(pair.second));

            // map the ent to an array index
            m_ent_to_component.emplace(ent, index);
            m_component_to_ent.emplace(index, ent);
        }
        m_additions.clear();
    }

private:
    std::unordered_map<size_t, size_t> m_ent_to_component;
    std::unordered_map<size_t, size_t> m_component_to_ent;
    std::vector<T> m_components;

    // <ent, component> for additions and removals
    std::vector<std::pair<uint, T>> m_additions;
    std::vector<std::pair<uint, T*>> m_removals;
};
