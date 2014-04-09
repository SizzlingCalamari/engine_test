
#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>

template<typename T>
class ComponentTable
{
    struct Entry
    {
        uint ent;
        T component;
    };
public:
    // Components are attached right away.
    // The ent indices are added to an additions list
    // for the current frame.
    void AttachComponent(uint ent, T* component)
    {
        auto it = m_ent_to_component.find(ent);
        if (it == m_ent_to_component.end())
        {
            // add the ent to the additions list
            m_additions.emplace_back(ent);

            auto index = m_components.size();

            // add the component
            Entry e;
            e.ent = ent;
            e.component = std::move(*component);
            m_components.emplace_back(e);

            // map the ent to an array index
            m_ent_to_component.emplace(ent, index);
            m_component_to_ent.emplace(index, ent);
        }
    }

    // Removing a component is queued until the next
    // call to CommitChanges.
    // The ent indices are added to a pending remove list.
    void RemoveComponent(uint ent)
    {
        auto it = m_ent_to_component.find(ent);
        if (it != m_ent_to_component.end())
        {
            m_removals.emplace_back(ent);
        }
    }

    bool HasComponent(uint ent)
    {
        return (m_ent_to_component.find(ent) != m_ent_to_component.end());
    }

    T* GetComponent(uint ent)
    {
        return &m_components[m_ent_to_component[ent]].component;
    }

    const std::vector<Entry>& GetComponentArray() const
    {
        return m_components;
    }

    const std::vector<uint>& GetAdditions() const
    {
        return m_additions;
    }

    const std::vector<uint>& GetRemovals() const
    {
        return m_removals;
    }

    void CommitChanges()
    {
        auto less_compare = [](std::pair<size_t, size_t> a, std::pair<size_t, size_t> b)
        {
            return a.first < b.first;
        };

        // Get removed component indices.
        std::vector<std::pair<size_t, size_t>> remove_pairs;
        remove_pairs.reserve(m_removals.size());
        {
            for (auto ent : m_removals)
            {
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
                auto &entry = m_components[removing_index];
                entry.component.FreeComponent();

                // if there is a valid replacement, replace
                if (replacement_index > 0)
                {
                    auto replacement_ent = m_component_to_ent[pair.second];

                    entry = std::move(m_components[replacement_index]);
                    m_ent_to_component[replacement_ent] = removing_index;
                    m_component_to_ent[removing_index] = replacement_ent;
                    m_component_to_ent.erase(replacement_index);
                }
                else
                {
                    // otherwise, remove the element mapping
                    // for the removed element
                    m_component_to_ent.erase(removing_index);
                }
            }

            for (auto &pair : remove_pairs)
            {
                m_components.pop_back();
            }
        }
        m_removals.clear();
        m_additions.clear();
    }

private:
    std::unordered_map<size_t, size_t> m_ent_to_component;
    std::unordered_map<size_t, size_t> m_component_to_ent;
    std::vector<Entry> m_components;

    std::vector<uint> m_additions;
    std::vector<uint> m_removals;
};
