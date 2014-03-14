
#pragma once

#include "Model.h"
#include "SceneGraphNode.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
/*
template<>
void SceneGraphNode<mat4>::UpdateData(
    matrix4x4& data, const matrix4x4& update)
{
    data.Transform(update);
}

template<>
void SceneGraphNode<matrix4x4>::ResetData(matrix4x4& data)
{
    data.SetIdentity();
}*/

class Renderable
{
public:
    void SetTransform(const glm::mat4& transform)
    {
        m_transform = transform;
    }

    const glm::mat4& GetTransform() const
    {
        return m_transform;
    }

    void LoadVerticies(const float* verticies, uint num_verticies)
    {
        m_model.LoadVerticies(verticies, num_verticies);
    }

    uint GetNumVerticies() const
    {
        return m_model.GetNumVerticies();
    }

    uint GetVertexBufferId() const
    {
        return m_model.GetVertexBufferId();
    }

private:
    //SceneGraphNode<matrix4x4> m_transform;
    glm::mat4 m_transform;
    Model m_model;
};
