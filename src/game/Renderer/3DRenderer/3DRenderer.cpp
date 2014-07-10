
#include "3DRenderer.h"

#include <iostream>
#include "ResourceLoader.h"
#include "ShaderManager.h"
#include "Camera.h"
#include "utils.h"

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

    m_shader_manager->CompileShaders(
        {"shaders/simplevertex.vert", "shaders/texturevertex.vert"},
        {"shaders/simplefragment.frag", "shaders/texturefragment.frag"},
    vertexShaders, fragmentShaders);

    m_colour_shader = m_shader_manager->CreateProgram();
    m_colour_shader.AttachShader(vertexShaders[0]);
    m_colour_shader.AttachShader(fragmentShaders[0]);
    bool linked = m_colour_shader.Link();
    assert(linked);

    m_texture_shader = m_shader_manager->CreateProgram();
    m_texture_shader.AttachShader(vertexShaders[1]);
    m_texture_shader.AttachShader(fragmentShaders[1]);
    linked = m_texture_shader.Link();
    assert(linked);
}

void Renderer3D::Shutdown()
{
    m_resourceLoader->UnloadResources();
    delete m_resourceLoader;
    m_resourceLoader = nullptr;

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &m_vao);
}

void Renderer3D::RenderScene(const Viewport* viewport, const Camera* cam, const std::vector<Renderable>& scene)
{
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

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto aspect = viewport->GetAspectRatio();
    auto pv = glm::perspective(glm::quarter_pi<float>(), aspect, 0.1f, 1000.0f) * cam->GetView();

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
        m_colour_shader.SetUniform("MVP", &mvp[0][0]);

        auto *mesh = m_resourceLoader->GetMesh(obj->mesh);
        GLsizei num_verticies = static_cast<GLsizei>(mesh->numVerticies);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBufferId);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            glDrawArrays(GL_TRIANGLES, 0, num_verticies);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    m_colour_shader.Unbind();

    // render using the texture shader
    m_texture_shader.Bind();
    for (auto &obj : m_texture_shader_cache)
    {
        auto mvp = pv * obj->transform;
        m_texture_shader.SetUniform("MVP", &mvp[0][0]);

        m_texture_shader.SetUniform("modelToWorld", &obj->transform);

        int texture_sampler = 0;
        m_texture_shader.SetUniform("myTextureSampler", &texture_sampler);

        glm::vec3 ambientColour(1.0f);
        m_texture_shader.SetUniform("directionalLight.colour", &ambientColour);

        float ambientIntensity = 0.2f;
        m_texture_shader.SetUniform("directionalLight.ambientIntensity", &ambientIntensity);

        glm::vec3 direction(-1.0f);
        m_texture_shader.SetUniform("directionalLight.direction", &direction);

        float diffuseIntensity = 0.5f;
        m_texture_shader.SetUniform("directionalLight.diffuseIntensity", &diffuseIntensity);

        m_texture_shader.SetUniform("eyePosition_worldspace", &cam->GetPosition());

        float specularIntensity = 1.0f;
        m_texture_shader.SetUniform("specularIntensity", &specularIntensity);

        float specularPower = 32.0f;
        m_texture_shader.SetUniform("specularPower", &specularPower);

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
