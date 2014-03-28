
#pragma once

#include "../Renderer/3DRenderer/3DRenderer.h"
#include "../Renderer/3DRenderer/Camera.h"
#include "ComponentTable.h"
#include "PhysicalComponent.h"

class Viewport;
class Scene;

class RenderProxy
{
public:
    RenderProxy(Renderer3D* renderer):
        m_renderer(renderer),
        m_physical_components(nullptr),
        m_graphical_components(nullptr)
    {
        assert(m_renderer);
    }

    void SetComponentTables(ComponentTable<PhysicalComponent>* physical,
                            ComponentTable<GraphicalComponent>* graphical)
    {
        m_physical_components = physical;
        m_graphical_components = graphical;
    }

    void RenderScene(const Viewport* viewport, uint camera, const std::vector<uint>& renderables)
    {
        assert(m_physical_components);
        assert(m_graphical_components);

        SetActiveCamera(camera);
        BuildScene(renderables);
        m_renderer->RenderScene(viewport, &m_camera, &m_scene);
    }

private:
    void SetActiveCamera(uint camera)
    {
        auto *physical = m_physical_components->GetComponent(camera);
        assert(physical);
        m_camera.CalcView(physical->forward, physical->position, physical->up);
    }

    void BuildScene(const std::vector<uint>& renderables)
    {
        m_scene.Clear();
        for (uint ent : renderables)
        {
            assert(m_physical_components->HasComponent(ent));
            assert(m_graphical_components->HasComponent(ent));

            auto *physical = m_physical_components->GetComponent(ent);
            auto *graphical = m_graphical_components->GetComponent(ent);

            // TODO: rotation, scale
            m_scene.AddRenderable(glm::translate(physical->position), graphical->mesh);
        }
    }

private:
    Renderer3D *m_renderer;
    ComponentTable<PhysicalComponent> *m_physical_components;
    ComponentTable<GraphicalComponent> *m_graphical_components;

    Camera m_camera;
    Scene m_scene;
};
