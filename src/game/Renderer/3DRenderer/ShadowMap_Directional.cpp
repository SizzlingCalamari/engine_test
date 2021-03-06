
#include "ShadowMap.h"
#include "GL/glew.h"
#include "Camera.h"
#include "Scene.h"
#include "ResourceLoader.h"

ShadowMapDirectional::ShadowMapDirectional():
    m_resourceLoader(nullptr),
    m_shadowMapShader(0),
    m_fbo(0),
    m_shadowMap(0),
    m_shadowMapWidth(0),
    m_shadowMapHeight(0),
    m_frustumDimensions(0.0f),
    m_biasPV(1.0f)
{
}

bool ShadowMapDirectional::Init(ResourceLoader* resourceLoader,
                                ShaderProgram shadowMapShader,
                                const glm::vec3& frustumDimensions,
                                uint shadowMapWidth, uint shadowMapHeight)
{
    m_resourceLoader = resourceLoader;
    m_shadowMapShader = std::move(shadowMapShader);
    m_shadowMapWidth = shadowMapWidth;
    m_shadowMapHeight = shadowMapHeight;
    m_frustumDimensions = frustumDimensions;

    if (m_fbo == 0)
    {
        glGenFramebuffers(1, &m_fbo);
    }
    if (m_shadowMap == 0)
    {
        glGenTextures(1, &m_shadowMap);
    }

    // bind and configure the shadowmap
    glBindTexture(GL_TEXTURE_2D, m_shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
                 shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // add linear filtering
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // GL_CLAMP is deprecated, so use GL_CLAMP_TO_EDGE
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // unbind the shadowmap
    glBindTexture(GL_TEXTURE_2D, 0);

    // bind the framebuffer and attach the shadowmap texture
    // as a depth attachment
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadowMap, 0);

    // disable colour buffer for this fbo
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    // make sure the framebuffer is complete
    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    // unbind the fbo
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    assert(status == GL_FRAMEBUFFER_COMPLETE);

    return (status == GL_FRAMEBUFFER_COMPLETE);
}

void ShadowMapDirectional::Free()
{
    if (m_fbo != 0)
    {
        glDeleteFramebuffers(1, &m_fbo);
    }
    if (m_shadowMap != 0)
    {
        glDeleteTextures(1, &m_shadowMap);
    }
}

void ShadowMapDirectional::RenderShadowMap(const glm::vec3& lightDirection,
                                           const Scene* scene)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_shadowMapWidth, m_shadowMapHeight);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glClear(GL_DEPTH_BUFFER_BIT);

    float halfWidth = m_frustumDimensions.x / 2.0f;
    float halfHeight = m_frustumDimensions.y / 2.0f;
    float halfDepth = m_frustumDimensions.z / 2.0f;
    auto projMatrix = glm::ortho(-halfWidth, halfWidth,
                                 -halfHeight, halfHeight,
                                 -halfDepth, halfDepth);

    auto right = glm::cross(lightDirection, glm::vec3(0.0f, 1.0f, 0.0f));
    auto up = glm::cross(right, lightDirection);
    auto viewMatrix = glm::lookAt(glm::vec3(0.0f), lightDirection, glm::normalize(up));
    auto pv = projMatrix * viewMatrix;

    // Scales by 0.5, then translates by +0.5
    // Causes homogeneous coordinates to transform
    // from [-1,1] to [0, 1]
    static glm::mat4 biasMatrix(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f);
    m_biasPV = biasMatrix * pv;

    m_shadowMapShader.Bind();

    for (auto &obj : scene->m_objects.GetDataArray())
    {
        auto *mesh = m_resourceLoader->GetMesh(obj.meshId);
        auto mvp = pv * obj.transform;
        m_shadowMapShader.SetUniform("g_depthMVP", &mvp[0][0]);

        GLsizei numVertices = static_cast<GLsizei>(mesh->numVertices);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBufferId);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray(0);
            glDrawArrays(GL_TRIANGLES, 0, numVertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    m_shadowMapShader.Unbind();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
