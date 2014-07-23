
#pragma once

#include "ShaderProgram.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

class ShaderProgram;
class ResourceLoader;
struct Scene;

class ShadowMapDirectional
{
public:
    ShadowMapDirectional();

    bool Init(ResourceLoader* resourceLoader,
              ShaderProgram shadowMapShader,
              const glm::vec3& frustumDimensions,
              uint shadowMapWidth, uint shadowMapHeight);

    void Free();

    void RenderShadowMap(const glm::vec3& lightDirection,
                         const Scene* scene);

    const glm::mat4& GetBiasDepthPV() const
    {
        return m_biasPV;
    }

    uint GetShadowMapId() const
    {
        return m_shadowMap;
    }

private:
    ResourceLoader* m_resourceLoader;
    ShaderProgram m_shadowMapShader;

    uint m_fbo;
    uint m_shadowMap;

    uint m_shadowMapWidth;
    uint m_shadowMapHeight;

    glm::vec3 m_frustumDimensions;

    glm::mat4x4 m_biasPV;
};
