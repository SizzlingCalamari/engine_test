
#pragma once

#include "../Renderer/3DRenderer/Camera.h"
#include "../Renderer/3DRenderer/Scene.h"

class Renderer3D;
class ResourceLoader;
class Viewport;

template<typename T> class ComponentTable;
struct PhysicalComponent;
struct GraphicalComponent;

struct BaseLight;
struct DirectionalLight;
struct SpotLight;
struct PointLight;
struct Material;

struct RenderObject
{
    enum RenderableType
    {
        MeshObject = 0,
        DirectionalLight,
        PointLight,
        SpotLight,
        Material
    };

    using PropertyMap = std::unordered_map<std::string, std::string>;

    RenderableType type;
    PropertyMap properties;
};

class RenderProxy
{
public:
    RenderProxy(Renderer3D* renderer);
    ~RenderProxy();

    void SetComponentTables(ComponentTable<PhysicalComponent>* physical,
                            ComponentTable<GraphicalComponent>* graphical);

    void RenderScene(const Viewport* viewport, uint camera);

    void Update();

    uint CreateRenderObject(const RenderObject& obj);

private:
    void SetActiveCamera(uint camera);

    void CheckEntChanges();

    void LoadRenderable(SceneNode* node, const PhysicalComponent* physical);
    void LoadRenderable(SceneNode* node, const GraphicalComponent* graphical);

    void ParseMeshObject(uint &mesh, const RenderObject& obj);
    void ParseBaseLight(BaseLight& light, const RenderObject& obj);
    void ParseDirectionalLight(DirectionalLight& light, const RenderObject& obj);
    void ParsePointLight(PointLight& light, const RenderObject& obj);
    void ParseSpotLight(SpotLight& light, const RenderObject& obj);
    void ParseMaterial(Material &mat, const RenderObject& obj);

private:
    Renderer3D *m_renderer;
    ResourceLoader *m_resource_loader;
    ComponentTable<PhysicalComponent> *m_physical_components;
    ComponentTable<GraphicalComponent> *m_graphical_components;

    Camera m_camera;
    Scene m_scene;

    class SceneResources;
    SceneResources* m_sceneResources;
};

inline void RenderProxy::SetComponentTables(ComponentTable<PhysicalComponent>* physical,
                                            ComponentTable<GraphicalComponent>* graphical)
{
    m_physical_components = physical;
    m_graphical_components = graphical;
}
