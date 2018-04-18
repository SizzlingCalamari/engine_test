
#pragma once

#include "GLContext.h"
#include "Viewport.h"
#include "ShaderProgram.h"
#include "ShadowMap.h"
#include <vector>

class Camera;
class ShaderManager;
class ResourceLoader;
class MaterialCache;
struct Scene;
struct SceneNode;

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

    ResourceLoader* GetResourceLoader() const;

    MaterialCache* GetMaterialCache() const;

private:
    void CreateSceneFbo(uint width, uint height);

private:
    GLContext m_glcontext;
    uint m_vao;

    uint m_fbo;
    uint m_colorAttach0;
    uint m_depthRbo;
    uint m_fbo_vbo;

    ResourceLoader *m_resourceLoader;

    ShaderManager *m_shader_manager;
    ShaderProgram m_texture_shader;
    ShaderProgram m_depth_prepass_shader;
    ShaderProgram m_skybox_shader;
    ShaderProgram m_post_process;

    std::vector<const SceneNode*> m_colour_shader_cache;
    std::vector<const SceneNode*> m_texture_shader_cache;

    Viewport m_fullview;

    ShadowMapDirectional m_shadowMapping;
};

inline ResourceLoader* Renderer3D::GetResourceLoader() const
{
    return m_resourceLoader;
}
