
#pragma once

#include "GLContext.h"
#include "Viewport.h"
#include "ShaderProgram.h"

class Camera;
class Scene;
class ShaderManager;

struct renderer3d_config
{
    uint x = 0;
    uint y = 0;
    uint width = 0;
    uint height = 0;
};

class Renderer3D
{
public:
    Renderer3D(void *GLContext);

    void Init(const renderer3d_config& config);

    void Shutdown();

    void RenderScene(const Viewport* viewport, const Camera* cam, const Scene* scene);

private:
    GLContext m_glcontext;
    uint m_vao;

    ShaderManager *m_shader_manager;
    ShaderProgram m_colour_shader;
    ShaderProgram m_texture_shader;

    Viewport m_fullview;
};
