
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

class Camera
{
public:
    void CalcProjection()
    {
        m_projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    }

    const glm::mat4& GetProjection() const
    {
        return m_projection;
    }

private:
    glm::mat4 m_projection;
};
