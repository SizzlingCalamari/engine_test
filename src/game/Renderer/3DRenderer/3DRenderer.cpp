
#include "3DRenderer.h"

#include <iostream>
#include "ResourceLoader.h"
#include "ShaderManager.h"
#include "Camera.h"
#include "utils.h"
#include "Light.h"
#include "Material.h"
#include "Scene.h"
#include <sstream>

static void STDCALL GLErrorCallback(
    GLenum source, GLenum type,
    GLuint id, GLenum severity,
    GLsizei length, const GLchar *msg,
    GLvoid *userdata )
{
    std::cout << msg << std::endl;
}

Renderer3D::Renderer3D(void *GLContext):
    m_glcontext(GLContext),
    m_shader_manager(nullptr),
    m_colour_shader(0),
    m_texture_shader(0)
{
}

void Renderer3D::Init(const renderer3d_config& config)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    GLContext::SetDebugMessageCallback(&GLErrorCallback);
    GLContext::EnableDepthTest(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_fullview.SetView(config.x, config.y, config.width, config.height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    m_resourceLoader = new ResourceLoader();

    m_shader_manager = m_glcontext.CreateShaderManager();

    std::vector<uint> vertexShaders;
    std::vector<uint> fragmentShaders;
    std::vector<uint> utilVertexShaders;
    std::vector<uint> utilFragmentShaders;

    m_shader_manager->CompileShaders(
        {"shaders/simplevertex.vert", "shaders/texturevertex.vert", "shaders/shadowmap.vert"},
        {"shaders/simplefragment.frag", "shaders/texturefragment.frag", "shaders/shadowmap.frag"},
        {"shaders/noise3D.glsl"},
        {"shaders/noise3D.glsl"},
        vertexShaders, fragmentShaders,
        utilVertexShaders, utilFragmentShaders);

    m_colour_shader = m_shader_manager->CreateProgram();
    m_colour_shader.AttachShader(vertexShaders[0]);
    m_colour_shader.AttachShader(fragmentShaders[0]);
    m_colour_shader.AttachShader(utilFragmentShaders[0]);
    bool linked = m_colour_shader.Link();
    assert(linked);

    m_texture_shader = m_shader_manager->CreateProgram();
    m_texture_shader.AttachShader(vertexShaders[1]);
    m_texture_shader.AttachShader(fragmentShaders[1]);
    m_texture_shader.AttachShader(utilFragmentShaders[0]);
    linked = m_texture_shader.Link();
    assert(linked);

    auto shadowMapShader = m_shader_manager->CreateProgram();
    shadowMapShader.AttachShader(vertexShaders[2]);
    shadowMapShader.AttachShader(fragmentShaders[2]);
    linked = shadowMapShader.Link();
    assert(linked);
    m_shadowMapping.Init(m_resourceLoader,
                         std::move(shadowMapShader),
                         glm::vec3{7000.0f, 7000.0f, 15000.0f},
                         4096, 4096);
}

void Renderer3D::Shutdown()
{
    m_shadowMapping.Free();

    m_resourceLoader->UnloadResources();
    delete m_resourceLoader;
    m_resourceLoader = nullptr;

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &m_vao);
}

void Renderer3D::RenderScene(const Viewport* viewport, const Camera* cam, const Scene* scene)
{
    const DirectionalLight* directionalLight = nullptr;
    {
        // shadow mapping depth render
        auto& directionalLights = scene->m_directionalLights.GetDataArray();
        if (directionalLights.size() > 0)
        {
            directionalLight = &directionalLights[0];
            // shaders only supports 1 directional light currently
            m_shadowMapping.RenderShadowMap(directionalLight->direction, scene);
        }
    }

    bool do_scissor = false;
    if (viewport)
    {
        do_scissor = (m_fullview != *viewport);
    }
    else
    {
        viewport = &m_fullview;
    }
    
    if (do_scissor)
    {
        glEnable(GL_SCISSOR_TEST);
    }
    viewport->SetActive();

    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto aspect = viewport->GetAspectRatio();
    auto pv = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 10000.0f) * cam->GetView();


    // render using the texture shader
    m_texture_shader.Bind();

    // directional lighting
    m_texture_shader.SetUniform("g_directionalLight.base.colour", &directionalLight->colour);
    m_texture_shader.SetUniform("g_directionalLight.base.ambientIntensity", &directionalLight->ambientIntensity);
    m_texture_shader.SetUniform("g_directionalLight.base.diffuseIntensity", &directionalLight->diffuseIntensity);
    m_texture_shader.SetUniform("g_directionalLight.direction", &directionalLight->direction);

    // spot lights
    auto &spotLights = scene->m_spotLights.GetDataArray();
    int numSpotLights = static_cast<int>(spotLights.size());
    m_texture_shader.SetUniform("g_numSpotLights", &numSpotLights);
    for (int i = 0; i < numSpotLights; ++i)
    {
        auto& spotLight = spotLights[i];

        std::stringstream ss;
        ss << "g_spotLights[" << i << "].";

        m_texture_shader.SetUniform(ss.str() + "base.base.colour", &spotLight.colour);
        m_texture_shader.SetUniform(ss.str() + "base.base.ambientIntensity", &spotLight.ambientIntensity);
        m_texture_shader.SetUniform(ss.str() + "base.base.diffuseIntensity", &spotLight.diffuseIntensity);
        m_texture_shader.SetUniform(ss.str() + "base.position", &spotLight.position);
        m_texture_shader.SetUniform(ss.str() + "base.attenuation.constant", &spotLight.attenuation.constant);
        m_texture_shader.SetUniform(ss.str() + "base.attenuation.linear", &spotLight.attenuation.linear);
        m_texture_shader.SetUniform(ss.str() + "base.attenuation.exponential", &spotLight.attenuation.exp);
        m_texture_shader.SetUniform(ss.str() + "coneDirection", &spotLight.coneDirection);
        m_texture_shader.SetUniform(ss.str() + "cosineConeAngle", &spotLight.cosineConeAngle);
    }

    // point lights
    auto &pointLights = scene->m_pointLights.GetDataArray();
    int numPointLights = static_cast<int>(pointLights.size());
    m_texture_shader.SetUniform("g_numPointLights", &numPointLights);
    for (int i = 0; i < numPointLights; ++i)
    {
        auto& light = pointLights[i];

        std::stringstream ss;
        ss << "g_pointLights[" << i << "].";

        m_texture_shader.SetUniform(ss.str() + "base.colour", &light.colour);
        m_texture_shader.SetUniform(ss.str() + "base.ambientIntensity", &light.ambientIntensity);
        m_texture_shader.SetUniform(ss.str() + "base.diffuseIntensity", &light.diffuseIntensity);
        m_texture_shader.SetUniform(ss.str() + "position", &light.position);
        m_texture_shader.SetUniform(ss.str() + "attenuation.constant", &light.attenuation.constant);
        m_texture_shader.SetUniform(ss.str() + "attenuation.linear", &light.attenuation.linear);
        m_texture_shader.SetUniform(ss.str() + "attenuation.exponential", &light.attenuation.exp);
    }

    int texture_sampler = 0;
    m_texture_shader.SetUniform("g_diffuseMapSampler", &texture_sampler);

    int depthSampler = 1;
    m_texture_shader.SetUniform("g_shadowMapSampler", &depthSampler);

    m_texture_shader.SetUniform("eyePosition_worldspace", &cam->GetPosition());

    for (auto &obj : scene->m_objects.GetDataArray())
    {
        auto mvp = pv * obj.transform;
        auto depthMVP = m_shadowMapping.GetBiasDepthPV() * obj.transform;
        m_texture_shader.SetUniform("MVP", &mvp[0][0]);
        m_texture_shader.SetUniform("g_depthMVP", &depthMVP[0][0]);
        m_texture_shader.SetUniform("modelToWorld", &obj.transform);

        auto *material = m_resourceLoader->GetMaterial(obj.materialId);
        m_texture_shader.SetUniform("g_diffuseColour", &material->diffuseSolidColour);
        m_texture_shader.SetUniform("g_noiseDiffuseMap", &material->noiseDiffuseMap);
        m_texture_shader.SetUniform("g_noiseBumpMap", &material->noiseBumpMap);
        m_texture_shader.SetUniform("g_celShaded", &material->celShaded);
        m_texture_shader.SetUniform("g_specularIntensity", &material->specularIntensity);
        m_texture_shader.SetUniform("g_specularPower", &material->specularPower);
        m_texture_shader.SetUniform("g_alphaTestValue", &material->alphaTestValue);
        bool usingDiffuseColour = false;
        if (material->diffuseMap == 0 && !material->noiseDiffuseMap)
        {
            usingDiffuseColour = true;
        }
        m_texture_shader.SetUniform("g_solidDiffuseColour", &usingDiffuseColour);

        auto *mesh = m_resourceLoader->GetMesh(obj.meshId);
        m_texture_shader.SetUniform("minAABB", &mesh->minAABB);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBufferId);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        if (mesh->uvBufferId > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mesh->uvBufferId);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray(1);
        }

        glBindBuffer(GL_ARRAY_BUFFER, mesh->normalBufferId);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);

        if (material->diffuseMap > 0)
        {
            auto *texture = m_resourceLoader->GetTexture(material->diffuseMap);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture->GetGLId());
        }

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_shadowMapping.GetShadowMapId());
            
        auto num_vertices = static_cast<GLsizei>(mesh->numVertices);
        glDrawArrays(GL_TRIANGLES, 0, num_vertices);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    m_texture_shader.Unbind();

    if (do_scissor)
    {
        glDisable(GL_SCISSOR_TEST);
    }
}
