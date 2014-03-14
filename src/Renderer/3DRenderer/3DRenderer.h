
#pragma once

#include "GLContext.h"
#include "ShaderManager.h"
#include "Camera.h"
#include "Scene.h"
#include <iostream>

#include "utils.h"

static void STDCALL GLErrorCallback(
    GLenum source, GLenum type,
    GLuint id, GLenum severity,
    GLsizei length, const GLchar *msg,
    GLvoid *userdata )
{
    std::cout << msg << std::endl;
}

class Renderer3D
{
public:
    Renderer3D(void *GLContext) :
        m_glcontext(GLContext),
        m_shader_manager(nullptr)
    {
        assert(GLContext);
        // glew needs to be initialized after 
        // the gl_createcontext call
        static RunOnce glew([]
        {
            glewExperimental = true;
            glewInit();
        });
    }

    void Init()
    {
        GLContext::SetDebugMessageCallback(&GLErrorCallback);
        GLContext::EnableDepthTest(GL_LESS);
        glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

        m_shader_manager = m_glcontext.CreateShaderManager();

        std::vector<Shader> vertexShaders;
        std::vector<Shader> fragmentShaders;

        m_shader_manager->CompileShaders(
            {"../src/shaders/simplevertex.vs", "../src/shaders/texturevertex.vs"},
            {"../src/shaders/simplefragment.fs", "../src/shaders/texturefragment.fs"},
        vertexShaders, fragmentShaders);

        m_colour_shader = m_shader_manager->CreateProgram();
        glAttachShader(m_colour_shader, vertexShaders[0]);
        glAttachShader(m_colour_shader, fragmentShaders[0]);
        glLinkProgram(m_colour_shader);
        {
            int res = GL_FALSE;
            glGetProgramiv(m_colour_shader, GL_LINK_STATUS, &res);
            bool linked = res == GL_TRUE;
            assert(linked);
        }
        m_mvp_id = glGetUniformLocation(m_colour_shader, "MVP");

        m_texture_shader = m_shader_manager->CreateProgram();
        glAttachShader(m_texture_shader, vertexShaders[1]);
        glAttachShader(m_texture_shader, fragmentShaders[1]);
        glLinkProgram(m_texture_shader);
        {
            int res = GL_FALSE;
            glGetProgramiv(m_texture_shader, GL_LINK_STATUS, &res);
            bool linked = res == GL_TRUE;
            assert(linked);
        }
        m_tex_id = glGetUniformLocation(m_texture_shader, "myTextureSampler");
    }

    void Shutdown()
    {
    }

    void RenderScene(Camera* cam, Scene* scene)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto pv = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f) * cam->GetView();

        glUseProgram(m_colour_shader);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        for (auto obj : scene->GetObjects())
        {
            auto mvp = pv * obj->GetTransform();
            
            glUniformMatrix4fv(m_mvp_id, 1, GL_FALSE, &mvp[0][0]);
            glBindBuffer(GL_ARRAY_BUFFER, obj->GetVertexBufferId());
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
                glDrawArrays(GL_TRIANGLES, 0, obj->GetNumVerticies());
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
        glUseProgram(0);
        //glUseProgram(m_texture_shader);
    }

private:
    GLContext m_glcontext;

    ShaderManager *m_shader_manager;
    uint m_colour_shader;
    uint m_texture_shader;

    uint m_mvp_id;
    uint m_tex_id;
};
