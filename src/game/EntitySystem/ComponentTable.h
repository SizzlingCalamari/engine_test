
#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>

template<typename T>
class ComponentTable
{
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
            m_components.emplace_back(std::move(*component));

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
    
    // Returns true if the passed in entity has a
    // component of type T.
    bool HasComponent(uint ent) const
    {
        return (m_ent_to_component.find(ent) != m_ent_to_component.end());
    }

    // Returns a const pointer to the component
    // belonging to the passed in entity.
    const T* PeekComponent(uint ent)
    {
        assert(HasComponent(ent));
        return &m_components[m_ent_to_component[ent]];
    }

    // Returns a copy of the component
    // belonging to the passed in entity.
    T GetComponent(uint ent)
    {
        assert(HasComponent(ent));
        return m_components[m_ent_to_component[ent]];
    }

    // Updates the component of the passed in entity
    // with the contents of the passed in component.
    void EditComponent(uint ent, T* component)
    {
        assert(HasComponent(ent));
        assert(component);

        // copy the updated component in
        m_components[m_ent_to_component[ent]] = std::move(*component);

        // add the ent to the edited list
        m_edits.emplace_back(ent);
    }

    const std::vector<T>& GetComponentArray() const
    {
        return m_components;
    }

    // Returns an array of the entity components
    // added since the last call to CommitChanges.
    const std::vector<uint>& GetAdditions() const
    {
        return m_additions;
    }

    // Returns an array of the entity components
    // removed since the last call to CommitChanges.
    const std::vector<uint>& GetRemovals() const
    {
        return m_removals;
    }

    // Returns an array of the entity components
    // edited since the last call to CommitChanges.
    const std::vector<uint>& GetEdits() const
    {
        return m_edits;
    }

    // Performs the entity removals and clears
    // the lists of additions, removals, and edits.
    // Makes a call to T::FreeComponent for each
    // removed component.
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
                auto &component = m_components[removing_index];
                component.FreeComponent();

                // if there is a valid replacement, replace
                if (replacement_index > 0)
                {
                    auto replacement_ent = m_component_to_ent[pair.second];

                    component = std::move(m_components[replacement_index]);
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

            // TODO: change this loop into a range based vector::erase
            for (auto &pair : remove_pairs)
            {
                m_components.pop_back();
            }
        }

        // nothing to do for edits

        // clear the arrays
        m_additions.clear();
        m_removals.clear();
        m_edits.clear();
    }

private:
    std::unordered_map<size_t, size_t> m_ent_to_component;
    std::unordered_map<size_t, size_t> m_component_to_ent;
    std::vector<T> m_components;

    std::vector<uint> m_additions;
    std::vector<uint> m_removals;
    std::vector<uint> m_edits;
};
