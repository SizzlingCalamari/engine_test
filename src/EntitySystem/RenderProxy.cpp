
#include "RenderProxy.h"

#include "../Renderer/3DRenderer/3DRenderer.h"

#include "ComponentTable.h"
#include "PhysicalComponent.h"
#include "GraphicalComponent.h"

#include <glm/gtx/transform.hpp>

void RenderProxy::RenderScene(const Viewport* viewport, uint camera)
{
    assert(m_physical_components);
    assert(m_graphical_components);

    SetActiveCamera(camera);
    m_renderer->RenderScene(viewport, &m_camera, m_scene);
}

void RenderProxy::Update()
{
    BuildScene();
}

void RenderProxy::SetActiveCamera(uint camera)
{
    auto *physical = m_physical_components->GetComponent(camera);
    assert(physical);
    m_camera.CalcView(physical->position, physical->orientation);
}

void RenderProxy::BuildScene()
{
    m_scene.clear();

    auto &graphical = m_graphical_components->GetComponentArray();
    for (auto &entry : graphical)
    {
        auto *physical = m_physical_components->GetComponent(entry.ent);
        auto *graphical = &entry.component;

        // TODO: scale
        auto translation = glm::translate(physical->position);
        auto orientation = glm::mat4_cast(physical->orientation);
        auto transform = translation * orientation;

        m_scene.emplace_back(Renderable{ transform, graphical->mesh });
    }
}
