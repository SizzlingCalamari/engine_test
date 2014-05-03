
#pragma once

#include <unordered_map>
#include <vector>

template<typename T>
class DataTable
{
public:
    // Add a mapping of id to data and
    // store the data by value.
    void AddData(uint id, T* data)
    {
        assert(!HasData(id));

        // get the index where the data will be
        uint index = static_cast<uint>(m_data.size());

        // add the data
        m_data.emplace_back(std::move(*data));

        // map the id to an array index
        m_id_to_data.emplace(id, index);
        m_data_to_id.emplace(index, id);
    }

    // Remove the mapping of id to data.
    void RemoveData(uint id)
    {
        assert(HasData(id));

        uint removal_index = m_id_to_data[id];
        uint replacement_index = static_cast<uint>(m_data.size() - 1);

        if (replacement_index > removal_index)
        {
            // assign replacement data to removed data
            m_data[removal_index] = std::move(m_data[replacement_index]);

            uint replacement_id = m_data_to_id[replacement_index];

            // swap the id to data mappings
            m_id_to_data[replacement_id] = removal_index;
            m_data_to_id[removal_index] = replacement_id;
        }
        else
        {
            assert(replacement_index == removal_index);
        }

        // erase the mappings and data
        m_data_to_id.erase(replacement_index);
        m_id_to_data.erase(id);
        m_data.pop_back();
    }
    
    // Returns true if the passed in entity has a
    // component of type T.
    bool HasData(uint id) const
    {
        return (m_id_to_data.find(id) != m_id_to_data.end());
    }

    // Returns a const pointer to the component
    // belonging to the passed in entity.
    T* GetData(uint id)
    {
        assert(HasData(id));
        return &m_data[m_id_to_data[id]];
    }

    const std::vector<T>& GetDataArray() const
    {
        return m_data;
    }

private:
    std::unordered_map<uint, uint> m_id_to_data;
    std::unordered_map<uint, uint> m_data_to_id;
    std::vector<T> m_data;
};
