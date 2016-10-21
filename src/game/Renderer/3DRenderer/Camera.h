
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mathutils.h"

class Camera
{
public:
    Camera():
        m_position(),
        m_orientation(),
        m_fovRad(glm::radians(70.0f)),
        m_nearZ(0.1f),
        m_farZ(10000.0f)
    {
    }

    void SetTransform(const glm::vec3& position, const glm::quat& orientation)
    {
        m_position = position;
        m_orientation = orientation;
    }

    glm::mat4 CalcViewProj(float aspectRatio) const
    {
        const glm::vec3 position = m_position;
        const glm::quat orientation = m_orientation;

        const glm::vec3 up = math::up(orientation);
        const glm::vec3 forward = math::forward(orientation);

        const glm::mat4 view = glm::lookAt(position, position + forward, up);
        const glm::mat4 perspective = glm::perspective(m_fovRad, aspectRatio, m_nearZ, m_farZ);
        return perspective * view;
    }

    const glm::vec3& GetPosition() const
    {
        return m_position;
    }

private:
    glm::vec3 m_position;
    glm::quat m_orientation;

    float m_fovRad;
    float m_nearZ;
    float m_farZ;
};
