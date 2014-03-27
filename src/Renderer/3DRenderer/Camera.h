
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

class Camera
{
public:
    void CalcView(const glm::vec3& forward, const glm::vec3& position, const glm::vec3& up)
    {
        m_view = glm::lookAt(forward, position, up);
    }

    const glm::mat4& GetView() const
    {
        return m_view;
    }

private:
    glm::mat4 m_view;
};
