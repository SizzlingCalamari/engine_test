
#pragma once

#include "Light.h"
#include <glm/mat4x4.hpp>
#include "DataTable.h"

struct SceneNode
{
    SceneNode():
        transform(1.0f),
        meshId(0),
        materialId(0)
    {
    }

    glm::mat4 transform;
    uint meshId;
    uint materialId;
};

struct Scene
{
    DataTable<SceneNode> m_objects;
    DataTable<DirectionalLight> m_directionalLights;
    DataTable<PointLight> m_pointLights;
    DataTable<SpotLight> m_spotLights;
};
