
#pragma once

#include "Renderable.h"
#include <vector>

class Scene
{
public:
    void AddRenderable(const glm::mat4& transform, const Mesh& mesh)
    {
        m_objects.emplace_back(Renderable{ transform, mesh });
    }

    const std::vector<Renderable>& GetRenderables() const
    {
        return m_objects;
    }

    void Clear()
    {
        m_objects.clear();
    }

private:
    std::vector<Renderable> m_objects;
};
