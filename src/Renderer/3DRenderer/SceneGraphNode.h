
#pragma once

#include <vector>

template<typename T>
class SceneGraphNode
{
public:
    inline void Propagate(const T& data)
    {
        for (auto &it : m_children)
        {
            it.Propagate(data);
        }
    }

    inline void QueuePropagation(const T& update)
    {
        UpdateData(m_propagation, update);
    }

    inline void PerformQueuedPropagation()
    {
        Propagate(m_propagation);
        ResetData(m_propagation);
    }

    inline T* operator->()
    {
        return &m_data;
    }

    inline T& operator*()
    {
        return m_data;
    }

    void AddChild(SceneGraphNode<T>* child)
    {
        assert(child);
        m_children.emplace_back(child);
    }

    void RemoveChild(SceneGraphNode<T>* child)
    {
        assert(child);
        std::remove(m_children.begin(), m_children.end(), child);
        m_children.pop_back();
    }

private:
    static inline void UpdateData(T& data, const T& update);
    static inline void ResetData(T& data);
    
private:
    T m_data;
    T m_propagation;
    std::vector<SceneGraphNode<T>*> m_children;
};
