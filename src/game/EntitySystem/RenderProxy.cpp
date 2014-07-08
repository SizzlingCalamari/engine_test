
#include "RenderProxy.h"

#include "../Renderer/3DRenderer/3DRenderer.h"
#include "../Renderer/3DRenderer/ResourceLoader.h"

#include "ComponentTable.h"
#include "PhysicalComponent.h"
#include "GraphicalComponent.h"

#include <glm/gtx/transform.hpp>

RenderProxy::RenderProxy(Renderer3D* renderer):
    m_renderer(renderer),
    m_resource_loader(renderer->GetResourceLoader()),
    m_physical_components(nullptr),
    m_graphical_components(nullptr)
{
    assert(renderer);
}

void RenderProxy::RenderScene(const Viewport* viewport, uint camera)
{
    SetActiveCamera(camera);
    m_renderer->RenderScene(viewport, &m_camera, m_scene.GetDataArray());
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
        auto *renderable = m_scene.GetData(ent);
        LoadRenderable(renderable, graphical);
    }

    auto &removals = m_graphical_components->GetRemovals();
    for (uint ent : removals)
    {
        // unload renderable?
        m_scene.RemoveData(ent);
    }

    auto &additions = m_graphical_components->GetAdditions();
    for (uint ent : additions)
    {
        auto *physical = m_physical_components->PeekComponent(ent);
        auto *graphical = m_graphical_components->PeekComponent(ent);

        Renderable r;
        LoadRenderable(&r, physical);
        LoadRenderable(&r, graphical);

        m_scene.AddData(ent, &r);
    }

    auto &physical_edits = m_physical_components->GetEdits();
    for (uint ent : physical_edits)
    {
        if (m_graphical_components->HasComponent(ent))
        {
            auto *renderable = m_scene.GetData(ent);
            auto *physical = m_physical_components->PeekComponent(ent);
            LoadRenderable(renderable, physical);
        }
    }
}

void RenderProxy::LoadRenderable(Renderable *r, const PhysicalComponent* physical)
{
    // Load the transformation
    // TODO: scale
    auto translation = glm::translate(physical->position);
    auto orientation = glm::mat4_cast(physical->orientation);
    r->transform = translation * orientation;
}

void RenderProxy::LoadRenderable(Renderable *r, const GraphicalComponent* graphical)
{
    // clear the renderable ids
    r->mesh = 0;
    r->texture = 0;

    // Load the mesh
    if (!graphical->mesh.empty())
    {
        r->mesh = m_resource_loader->LoadResource(graphical->mesh);
    }

    // Load the texture
    if (!graphical->texture.empty())
    {
        r->texture = m_resource_loader->LoadResource(graphical->texture);
    }
}
