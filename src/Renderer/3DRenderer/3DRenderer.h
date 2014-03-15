
#pragma once

#include "GLContext.h"

class Camera;
class Scene;
class ShaderManager;

class Renderer3D
{
public:
    Renderer3D(void *GLContext);

    void Init();

    void Shutdown();

    void RenderScene(const Camera* cam, const Scene* scene);

private:
    GLContext m_glcontext;
    uint m_vao;

    ShaderManager *m_shader_manager;
    uint m_colour_shader;
    uint m_texture_shader;

    uint m_mvp_id;
    uint m_tex_id;
};
