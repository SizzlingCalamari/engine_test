
#pragma once

#include "ShaderProgram.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

class ShaderProgram;
struct SceneNode;

class ShadowMapDirectional
{
public:
    ShadowMapDirectional();

    bool Init(ShaderProgram shadowMapShader,
              uint windowWidth, uint windowHeight,
              uint shadowMapWidth, uint shadowMapHeight);

    void Free();

    void RenderShadowMap(const glm::vec3& lightDirection,
                         const std::vector<SceneNode>& scene);

    const glm::mat4& GetBiasDepthPV() const
    {
        return m_biasPV;
    }

    uint GetShadowMapId() const
    {
        return m_shadowMap;
    }

private:
    ShaderProgram m_shadowMapShader;

    uint m_fbo;
    uint m_shadowMap;

    uint m_shadowMapWidth;
    uint m_shadowMapHeight;

    uint m_windowWidth;
    uint m_windowHeight;

    glm::mat4x4 m_biasPV;
};
