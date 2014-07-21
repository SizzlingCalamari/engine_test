
#include "3DRenderer.h"

#include <iostream>
#include "ResourceLoader.h"
#include "ShaderManager.h"
#include "Camera.h"
#include "utils.h"
#include "Light.h"
#include "Material.h"
#include "Scene.h"

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

    m_fullview.SetView(config.x, config.y, config.width, config.height);
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    m_resourceLoader = new ResourceLoader();

    m_shader_manager = m_glcontext.CreateShaderManager();

    std::vector<uint> vertexShaders;
    std::vector<uint> fragmentShaders;
    std::vector<uint> utilShaders;

    m_shader_manager->CompileShaders(
        {"shaders/simplevertex.vert", "shaders/texturevertex.vert", "shaders/shadowmap.vert"},
        {"shaders/simplefragment.frag", "shaders/texturefragment.frag", "shaders/shadowmap.frag"},
        {"shaders/noise3D.glsl"},
        vertexShaders, fragmentShaders, utilShaders);

    m_colour_shader = m_shader_manager->CreateProgram();
    m_colour_shader.AttachShader(vertexShaders[0]);
    m_colour_shader.AttachShader(fragmentShaders[0]);
    m_colour_shader.AttachShader(utilShaders[0]);
    bool linked = m_colour_shader.Link();
    assert(linked);

    m_texture_shader = m_shader_manager->CreateProgram();
    m_texture_shader.AttachShader(vertexShaders[1]);
    m_texture_shader.AttachShader(fragmentShaders[1]);
    linked = m_texture_shader.Link();
    assert(linked);

    auto shadowMapShader = m_shader_manager->CreateProgram();
    shadowMapShader.AttachShader(vertexShaders[2]);
    shadowMapShader.AttachShader(fragmentShaders[2]);
    linked = shadowMapShader.Link();
    assert(linked);
    m_shadowMapping.Init(std::move(shadowMapShader),
                         config.width, config.height, 4096, 4096);
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

void Renderer3D::RenderScene(const Viewport* viewport, const Camera* cam, const std::vector<Renderable>& scene)
{
    DirectionalLight directionalLight;
    directionalLight.colour = glm::vec3(1.0f);
    directionalLight.ambientIntensity = 0.1f;
    directionalLight.diffuseIntensity = 0.2f;
    directionalLight.direction = glm::vec3(-1.0f);

    // shadow mapping depth render
    {
        std::vector<SceneNode> sceneNodes;
        sceneNodes.reserve(scene.size());
        for (auto &obj : scene)
        {
            const Mesh *mesh = nullptr;
            if (obj.mesh > 0)
            {
                mesh = m_resourceLoader->GetMesh(obj.mesh);
            }
            const Texture *texture = nullptr;
            if (obj.texture > 0)
            {
                texture = m_resourceLoader->GetTexture(obj.texture);
            }
            sceneNodes.emplace_back(SceneNode{ obj.transform, mesh, texture });
        }
        m_shadowMapping.RenderShadowMap(directionalLight.direction, sceneNodes);
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

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto aspect = viewport->GetAspectRatio();
    auto pv = glm::perspective(glm::quarter_pi<float>(), aspect, 0.1f, 10000.0f) * cam->GetView();

    // categorize the renderables by shader
    m_colour_shader_cache.clear();
    m_texture_shader_cache.clear();
    for (auto &obj : scene)
    {
        if (obj.texture > 0)
        {
            m_texture_shader_cache.emplace_back(&obj);
        }
        else
        {
            m_colour_shader_cache.emplace_back(&obj);
        }
    }

    // render using the colour shader
    m_colour_shader.Bind();
    for (auto obj : m_colour_shader_cache)
    {
        auto mvp = pv * obj->transform;
        m_colour_shader.SetUniform("MVP", &mvp);

        auto *mesh = m_resourceLoader->GetMesh(obj->mesh);
        GLsizei num_verticies = static_cast<GLsizei>(mesh->numVerticies);

        m_colour_shader.SetUniform("minAABB", &mesh->minAABB);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBufferId);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            glDrawArrays(GL_TRIANGLES, 0, num_verticies);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    m_colour_shader.Unbind();

    // render using the texture shader
    m_texture_shader.Bind();

    m_texture_shader.SetUniform("g_directionalLight.base.colour", &directionalLight.colour);
    m_texture_shader.SetUniform("g_directionalLight.base.ambientIntensity", &directionalLight.ambientIntensity);
    m_texture_shader.SetUniform("g_directionalLight.base.diffuseIntensity", &directionalLight.diffuseIntensity);
    m_texture_shader.SetUniform("g_directionalLight.direction", &directionalLight.direction);

    int numPointLights = 1;
    m_texture_shader.SetUniform("g_numPointLights", &numPointLights);

    PointLight light;
    light.colour = glm::vec3(0.0f, 0.0f, 1.0f);
    light.ambientIntensity = 0.0f;
    light.diffuseIntensity = 0.3f;
    light.position = glm::vec3(-50.0f, 0.0f, -50.0f);

    m_texture_shader.SetUniform("g_pointLights[0].base.colour", &light.colour);
    m_texture_shader.SetUniform("g_pointLights[0].base.ambientIntensity", &light.ambientIntensity);
    m_texture_shader.SetUniform("g_pointLights[0].base.diffuseIntensity", &light.diffuseIntensity);
    m_texture_shader.SetUniform("g_pointLights[0].position", &light.position);
    m_texture_shader.SetUniform("g_pointLights[0].attenuation.constant", &light.attenuation.constant);
    m_texture_shader.SetUniform("g_pointLights[0].attenuation.linear", &light.attenuation.linear);
    m_texture_shader.SetUniform("g_pointLights[0].attenuation.exponential", &light.attenuation.exp);

    int numSpotLights = 1;
    m_texture_shader.SetUniform("g_numSpotLights", &numSpotLights);

    SpotLight spotLight;
    spotLight.colour = glm::vec3(0.0f, 1.0f, 0.0f);
    spotLight.ambientIntensity = 0.0f;
    spotLight.diffuseIntensity = 0.3f;
    spotLight.position = glm::vec3(50.0f, 0.0f, -50.0f);
    spotLight.coneDirection = (glm::vec3(0.0f) - spotLight.position);
    spotLight.cosineConeAngle = glm::cos(20.0f);

    m_texture_shader.SetUniform("g_spotLights[0].base.base.colour", &spotLight.colour);
    m_texture_shader.SetUniform("g_spotLights[0].base.base.ambientIntensity", &spotLight.ambientIntensity);
    m_texture_shader.SetUniform("g_spotLights[0].base.base.diffuseIntensity", &spotLight.diffuseIntensity);
    m_texture_shader.SetUniform("g_spotLights[0].base.position", &spotLight.position);
    m_texture_shader.SetUniform("g_spotLights[0].base.attenuation.constant", &spotLight.attenuation.constant);
    m_texture_shader.SetUniform("g_spotLights[0].base.attenuation.linear", &spotLight.attenuation.linear);
    m_texture_shader.SetUniform("g_spotLights[0].base.attenuation.exponential", &spotLight.attenuation.exp);
    m_texture_shader.SetUniform("g_spotLights[0].coneDirection", &spotLight.coneDirection);
    m_texture_shader.SetUniform("g_spotLights[0].cosineConeAngle", &spotLight.cosineConeAngle);

    Material material;
    material.specularIntensity = 1.0f;
    material.specularPower = 32.0f;

    m_texture_shader.SetUniform("g_materialProps.specularIntensity", &material.specularIntensity);
    m_texture_shader.SetUniform("g_materialProps.specularPower", &material.specularPower);

    int texture_sampler = 0;
    m_texture_shader.SetUniform("myTextureSampler", &texture_sampler);

    int depthSampler = 1;
    m_texture_shader.SetUniform("g_shadowMapSampler", &depthSampler);

    for (auto &obj : m_texture_shader_cache)
    {
        auto mvp = pv * obj->transform;
        auto depthMVP = m_shadowMapping.GetBiasDepthPV() * obj->transform;
        m_texture_shader.SetUniform("MVP", &mvp[0][0]);
        m_texture_shader.SetUniform("g_depthMVP", &depthMVP[0][0]);
        m_texture_shader.SetUniform("modelToWorld", &obj->transform);
        m_texture_shader.SetUniform("eyePosition_worldspace", &cam->GetPosition());

        auto *mesh = m_resourceLoader->GetMesh(obj->mesh);
        auto *texture = m_resourceLoader->GetTexture(obj->texture);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBufferId);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->uvBufferId);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->normalBufferId);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture->GetGLId());

        glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_shadowMapping.GetShadowMapId());
            
        auto num_verticies = static_cast<GLsizei>(mesh->numVerticies);
        glDrawArrays(GL_TRIANGLES, 0, num_verticies);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    m_texture_shader.Unbind();

    if (do_scissor)
    {
        glDisable(GL_SCISSOR_TEST);
    }
}
