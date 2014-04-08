
#pragma once

#include "../Renderer/3DRenderer/3DRenderer.h"
#include "../Renderer/3DRenderer/Camera.h"
#include "../Renderer/3DRenderer/Renderable.h"
#include "ComponentTable.h"
#include "PhysicalComponent.h"
#include <vector>

class Viewport;

class RenderProxy
{
public:
    RenderProxy(Renderer3D* renderer = nullptr):
        m_renderer(renderer),
        m_physical_components(nullptr),
        m_graphical_components(nullptr)
    {
    }

    void SetComponentTables(ComponentTable<PhysicalComponent>* physical,
                            ComponentTable<GraphicalComponent>* graphical)
    {
        m_physical_components = physical;
        m_graphical_components = graphical;
    }

    void RenderScene(const Viewport* viewport, uint camera)
    {
        assert(m_physical_components);
        assert(m_graphical_components);

        SetActiveCamera(camera);
        m_renderer->RenderScene(viewport, &m_camera, m_scene);
    }

    void Update()
    {
        BuildScene();
    }

private:
    void SetActiveCamera(uint camera)
    {
        auto *physical = m_physical_components->GetComponent(camera);
        assert(physical);
        m_camera.CalcView(physical->position, physical->orientation);
    }

    void BuildScene()
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

private:
    Renderer3D *m_renderer;
    ComponentTable<PhysicalComponent> *m_physical_components;
    ComponentTable<GraphicalComponent> *m_graphical_components;

    Camera m_camera;
    std::vector<Renderable> m_scene;
};
