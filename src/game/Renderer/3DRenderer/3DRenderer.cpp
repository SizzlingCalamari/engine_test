
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
    if(severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    {
        return;
    }
    std::cout << msg << std::endl;
}

Renderer3D::Renderer3D(void *GLContext):
    m_glcontext(GLContext),
    m_vao(0),
    m_fbo(0),
    m_colorAttach0(0),
    m_depthRbo(0),
    m_fbo_vbo(0),
    m_shader_manager(nullptr),
    m_texture_shader(0),
    m_depth_prepass_shader(0),
    m_skybox_shader(0),
    m_post_process(0)
{
}

void Renderer3D::Init(const renderer3d_config& config)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    GLContext::SetDebugMessageCallback(&GLErrorCallback);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    CreateSceneFbo(config.width, config.height);

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
        {"shaders/texturevertex.vert", "shaders/shadowmap.vert", "shaders/lambert.vert", "shaders/skybox.vert", "shaders/wave_post.vert"},
        {"shaders/texturefragment.frag", "shaders/shadowmap.frag", "shaders/lambert.frag", "shaders/skybox.frag", "shaders/wave_post.frag"},
        {"shaders/noise3D.glsl"},
        {"shaders/noise3D.glsl"},
        vertexShaders, fragmentShaders,
        utilVertexShaders, utilFragmentShaders);

    m_texture_shader = m_shader_manager->CreateProgram();
    m_texture_shader.AttachShader(vertexShaders[0]);
    m_texture_shader.AttachShader(fragmentShaders[0]);
    m_texture_shader.AttachShader(utilFragmentShaders[0]);
    bool linked = m_texture_shader.Link();
    assert(linked);

    auto shadowMapShader = m_shader_manager->CreateProgram();
    shadowMapShader.AttachShader(vertexShaders[1]);
    shadowMapShader.AttachShader(fragmentShaders[1]);
    linked = shadowMapShader.Link();
    assert(linked);
    m_depth_prepass_shader = shadowMapShader;
    m_shadowMapping.Init(m_resourceLoader,
                         std::move(shadowMapShader),
                         glm::vec3{7000.0f, 7000.0f, 15000.0f},
                         4096, 4096);

    m_skybox_shader = m_shader_manager->CreateProgram();
    m_skybox_shader.AttachShader(vertexShaders[3]);
    m_skybox_shader.AttachShader(fragmentShaders[3]);
    linked = m_skybox_shader.Link();
    assert(linked);

    m_post_process = m_shader_manager->CreateProgram();
    m_post_process.AttachShader(vertexShaders[4]);
    m_post_process.AttachShader(fragmentShaders[4]);
    linked = m_post_process.Link();
    assert(linked);
}

void Renderer3D::Shutdown()
{
    m_shadowMapping.Free();

    m_resourceLoader->UnloadResources();
    delete m_resourceLoader;
    m_resourceLoader = nullptr;

    glDeleteBuffers(1, &m_fbo_vbo);
    glDeleteRenderbuffers(1, &m_depthRbo);
    glDeleteTextures(1, &m_colorAttach0);
    glDeleteFramebuffers(1, &m_fbo);

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

    // Render to the fbo
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float aspectRatio = viewport->GetAspectRatio();
    const glm::mat4 proj = cam->CalcProj(aspectRatio);
    const glm::mat4 pv = proj * cam->CalcView();

    {
        m_skybox_shader.Bind();

        const glm::mat4 invProj = glm::inverse(proj);
        glm::mat4 triCoords(
            glm::vec4(-1.0f, -1.0f, 0.0f, 0.0f),
            glm::vec4(3.0f, -1.0f, 0.0f, 0.0f),
            glm::vec4(-1.0f, 3.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
        triCoords = invProj * triCoords;
        m_skybox_shader.SetUniform("TriCoords[0]", &triCoords[0]);
        m_skybox_shader.SetUniform("TriCoords[1]", &triCoords[1]);
        m_skybox_shader.SetUniform("TriCoords[2]", &triCoords[2]);

        glDepthMask(GL_FALSE);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glDepthMask(GL_TRUE);
        m_skybox_shader.Unbind();
    }

    // Perform depth prepass to help with overdraw in lighting calcs
    /*m_depth_prepass_shader.Bind();
    for (auto &obj : scene->m_objects.GetDataArray())
    {
        const glm::mat4 mvp = pv * obj.transform;
        m_depth_prepass_shader.SetUniform("g_depthMVP", &mvp[0][0]);

        auto *mesh = m_resourceLoader->GetMesh(obj.meshId);
        auto num_vertices = static_cast<GLsizei>(mesh->numVertices);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBufferId);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray(0);
            glDrawArrays(GL_TRIANGLES, 0, num_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    m_depth_prepass_shader.Unbind();*/

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
        m_texture_shader.SetUniform("cUVTransform", &material->uvTransform[0][0]);
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

    // Post process and render to screen framebuffer0
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_post_process.Bind();

    const int fbo_sampler = 0;
    m_post_process.SetUniform("g_fboSampler", &fbo_sampler);

    static float g_time = 0.0f;

    g_time += 0.01f;
    m_post_process.SetUniform("g_time", &g_time);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_colorAttach0);

    glBindBuffer(GL_ARRAY_BUFFER, m_fbo_vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    m_post_process.Unbind();

    if (do_scissor)
    {
        glDisable(GL_SCISSOR_TEST);
    }
}

void Renderer3D::CreateSceneFbo(uint width, uint height)
{
    // Color attachment
    {
        if (m_colorAttach0 == 0)
        {
            glGenTextures(1, &m_colorAttach0);
        }

        glBindTexture(GL_TEXTURE_2D, m_colorAttach0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8,
                        width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // 24-bit Depth attachment
    {
        GLint maxRenderBufferSize;
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize);
        assert(width <= maxRenderBufferSize);
        assert(height <= maxRenderBufferSize);

        if (m_depthRbo == 0)
        {
            glGenRenderbuffers(1, &m_depthRbo);
        }

        glBindRenderbuffer(GL_RENDERBUFFER, m_depthRbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    // Fbo
    {
        if (m_fbo == 0)
        {
            glGenFramebuffers(1, &m_fbo);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorAttach0, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRbo);

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        assert(status == GL_FRAMEBUFFER_COMPLETE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // 
    const GLfloat fbo_verts[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
    };
    glGenBuffers(1, &m_fbo_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fbo_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_verts), fbo_verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    
}
