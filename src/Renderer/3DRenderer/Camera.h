
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mathutils.h"

class Camera
{
public:
    void CalcView(const glm::vec3& position, const glm::quat& orientation)
    {
        auto up = math::up(orientation);
        auto forward = math::forward(orientation);

        m_view = glm::lookAt(position, position + forward, up);
    }

    const glm::mat4& GetView() const
    {
        return m_view;
    }

private:
    glm::mat4 m_view;
};
