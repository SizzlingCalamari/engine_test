
#pragma once

#include "GLContext.h"
#include "Viewport.h"

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
    uint m_colour_shader;
    uint m_texture_shader;

    uint m_mvp_id;
    uint m_tex_id;

    Viewport m_fullview;
};
