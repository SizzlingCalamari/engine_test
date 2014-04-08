
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera
{
public:
    void CalcView(const glm::vec3& position, const glm::quat& orientation)
    {
        auto conj = glm::conjugate(orientation);
        glm::vec3 up = orientation * glm::vec3(0.0f, 1.0f, 0.0f) * conj;
        glm::vec3 forward = orientation * glm::vec3(0.0f, 0.0f, 1.0f) * conj;

        m_view = glm::lookAt(position, position + forward, up);
    }

    const glm::mat4& GetView() const
    {
        return m_view;
    }

private:
    glm::mat4 m_view;
};
