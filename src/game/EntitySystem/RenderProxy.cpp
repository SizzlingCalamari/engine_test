
#include "RenderProxy.h"

#include "../Renderer/3DRenderer/3DRenderer.h"
#include "../Renderer/3DRenderer/ResourceLoader.h"
#include "../Renderer/3DRenderer/Scene.h"

#include "ComponentTable.h"
#include "PhysicalComponent.h"
#include "GraphicalComponent.h"
#include "StringParsing.h"

#include <glm/gtx/transform.hpp>
#include <iostream>

class RenderProxy::SceneResources
{
public:
    SceneResources():
        m_curResourceId(0)
    {
    }

    uint AddDirectionalLight(DirectionalLight& light)
    {
        m_directionalLights.AddData(++m_curResourceId, &light);
        return m_curResourceId;
    }

    uint AddPointLight(PointLight& light)
    {
        m_pointLights.AddData(++m_curResourceId, &light);
        return m_curResourceId;
    }

    uint AddSpotLight(SpotLight& light)
    {
        m_spotLights.AddData(++m_curResourceId, &light);
        return m_curResourceId;
    }

    uint m_curResourceId;
    DataTable<DirectionalLight> m_directionalLights;
    DataTable<PointLight> m_pointLights;
    DataTable<SpotLight> m_spotLights;
};

RenderProxy::RenderProxy(Renderer3D* renderer):
    m_renderer(renderer),
    m_resource_loader(renderer->GetResourceLoader()),
    m_physical_components(nullptr),
    m_graphical_components(nullptr),
    m_sceneResources(new SceneResources())
{
    assert(renderer);
}

RenderProxy::~RenderProxy()
{
    delete m_sceneResources;
}

void RenderProxy::RenderScene(const Viewport* viewport, uint camera)
{
    SetActiveCamera(camera);
    m_renderer->RenderScene(viewport, &m_camera, &m_scene);
}

uint RenderProxy::CreateRenderObject(RenderObject& obj)
{
    uint id = 0xffff;
    switch (obj.type)
    {
    case RenderObject::MeshObject:
        {
            uint meshId = 0;
            ParseMeshObject(meshId, obj);
            id = meshId;
        }
        break;
    case RenderObject::DirectionalLight:
        {
            DirectionalLight light;
            ParseDirectionalLight(light, obj);
            id = m_sceneResources->AddDirectionalLight(light);
        }
        break;
    case RenderObject::SpotLight:
        {
            SpotLight light;
            ParseSpotLight(light, obj);
            id = m_sceneResources->AddSpotLight(light);
        }
        break;
    case RenderObject::PointLight:
        {
            PointLight light;
            ParsePointLight(light, obj);
            id = m_sceneResources->AddPointLight(light);
        }
        break;
    case RenderObject::Material:
        {
            Material mat;
            ParseMaterial(mat, obj);
            id = m_resource_loader->AddMaterial(mat);
        }
        break;
    default:
        break;
    }

    return id;
}

void RenderProxy::Update()
{
    CheckEntChanges();
}

void RenderProxy::SetActiveCamera(uint camera)
{
    auto *physical = m_physical_components->PeekComponent(camera);
    m_camera.CalcView(physical->position, physical->orientation);
}

void RenderProxy::CheckEntChanges()
{
    auto &edits = m_graphical_components->GetEdits();
    for (uint ent : edits)
    {
        auto *graphical = m_graphical_components->PeekComponent(ent);
        auto *node = m_scene.m_objects.GetData(ent);
        LoadRenderable(node, graphical);
    }

    auto &removals = m_graphical_components->GetRemovals();
    for (uint ent : removals)
    {
        // unload renderable?
        m_scene.m_objects.RemoveData(ent);
    }

    auto &additions = m_graphical_components->GetAdditions();
    for (uint ent : additions)
    {
        auto *physical = m_physical_components->PeekComponent(ent);
        auto *graphical = m_graphical_components->PeekComponent(ent);

        SceneNode node;
        LoadRenderable(&node, physical);
        LoadRenderable(&node, graphical);
        if (node.meshId > 0)
        {
            m_scene.m_objects.AddData(ent, &node);
        }
    }

    auto &physical_edits = m_physical_components->GetEdits();
    for (uint ent : physical_edits)
    {
        if (m_graphical_components->HasComponent(ent))
        {
            auto *physical = m_physical_components->PeekComponent(ent);
            SceneNode *node = m_scene.m_objects.GetData(ent);
            LoadRenderable(node, physical);
        }
    }
}

void RenderProxy::LoadRenderable(SceneNode* node, const PhysicalComponent* physical)
{
    // Load the transformation
    // TODO: scale
    auto translation = glm::translate(physical->position);
    auto orientation = glm::mat4_cast(physical->orientation);
    node->transform = translation * orientation;
}

void RenderProxy::LoadRenderable(SceneNode* node, const GraphicalComponent* graphical)
{
    auto meshId = graphical->mesh;
    auto matId = graphical->material;
    // load mesh + material into node
    if (meshId > 0)
    {
        node->meshId = meshId;
        if (matId > 0)
        {
            node->materialId = matId;
        }
    }
    // update entries for lights
    if (graphical->directionalLight > 0)
    {
        uint id = graphical->directionalLight;
        auto *light = m_sceneResources->m_directionalLights.PeekData(id);
        m_scene.m_directionalLights.UpdateOrAddData(id, light);
    }
    if (graphical->pointLight > 0)
    {
        uint id = graphical->pointLight;
        auto *light = m_sceneResources->m_pointLights.PeekData(id);
        m_scene.m_pointLights.UpdateOrAddData(id, light);
    }
    if (graphical->spotLight > 0)
    {
        uint id = graphical->spotLight;
        auto *light = m_sceneResources->m_spotLights.PeekData(id);
        m_scene.m_spotLights.UpdateOrAddData(id, light);
    }
}

template<typename T>
bool FindAndLoad(const RenderObject::PropertyMap& props,
                 const std::string& str, T& out)
{
    out = T();

    auto it = props.find(str);
    if (it == props.end())
    {
        return false;
    }

    bool result = StringTo(it->second, out);
    if (!result)
    {
        std::cout << "Error parsing property \"" << str << "\"" << std::endl;
    }
    return result;
}

template<>
bool FindAndLoad(const RenderObject::PropertyMap& props,
                 const std::string& str, std::string& out)
{
    out.clear();
    auto it = props.find(str);
    if (it == props.end())
    {
        return false;
    }
    out = it->second;
    return true;
}

void RenderProxy::ParseMeshObject(uint &mesh, const RenderObject& obj)
{
    auto& props = obj.properties;
    std::string temp;
    if (FindAndLoad(props, "meshFile", temp))
    {
        mesh = m_resource_loader->LoadResource(temp);
    }
}

void RenderProxy::ParseBaseLight(BaseLight& light, const RenderObject& obj)
{
    auto& props = obj.properties;
    FindAndLoad(props, "colour", light.colour);
    FindAndLoad(props, "ambientIntensity", light.ambientIntensity);
    FindAndLoad(props, "diffuseIntensity", light.diffuseIntensity);
}

void RenderProxy::ParseDirectionalLight(DirectionalLight& light, const RenderObject& obj)
{
    ParseBaseLight(light, obj);
    auto& props = obj.properties;
    FindAndLoad(props, "direction", light.direction);
}

void RenderProxy::ParsePointLight(PointLight& light, const RenderObject& obj)
{
    ParseBaseLight(light, obj);
    auto& props = obj.properties;
    FindAndLoad(props, "position", light.position);
    FindAndLoad(props, "attenuation.constant", light.attenuation.constant);
    FindAndLoad(props, "attenuation.linear", light.attenuation.linear);
    FindAndLoad(props, "attenuation.exp", light.attenuation.exp);
}

void RenderProxy::ParseSpotLight(SpotLight& light, const RenderObject& obj)
{
    ParsePointLight(light, obj);
    auto& props = obj.properties;
    FindAndLoad(props, "coneDirection", light.coneDirection);
    FindAndLoad(props, "cosineConeAngle", light.cosineConeAngle);
}

void RenderProxy::ParseMaterial(Material &mat, const RenderObject& obj)
{
    auto& props = obj.properties;
    std::string temp;
    if (FindAndLoad(props, "diffuseMapFile", temp))
    {
        mat.diffuseMap = m_resource_loader->LoadResource(temp);
    }
    FindAndLoad(props, "diffuseSolidColour", mat.diffuseSolidColour);
    FindAndLoad(props, "specularIntensity", mat.specularIntensity);
    FindAndLoad(props, "specularPower", mat.specularPower);
    FindAndLoad(props, "noiseDiffuseMap", mat.noiseDiffuseMap);
    FindAndLoad(props, "noiseBumpMap", mat.noiseBumpMap);
    FindAndLoad(props, "celShaded", mat.celShaded);
}
