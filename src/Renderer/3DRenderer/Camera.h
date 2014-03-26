
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

class Camera
{
public:
    void CalcView()
    {
        m_view = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    }

    const glm::mat4& GetView() const
    {
        return m_view;
    }

private:
    glm::mat4 m_view;
};
