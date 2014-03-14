
#pragma once

#include "Renderable.h"
#include <vector>

class Scene
{
public:
    void AddObject(Renderable* r)
    {
        m_objects.emplace_back(r);
    }

    const std::vector<Renderable*>& GetObjects() const
    {
        return m_objects;
    }

    int GetNumObjects() const
    {
        return m_objects.size();
    }

private:
    std::vector<Renderable*> m_objects;
};
