
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
        m_renderer(renderer)
    {
    }

    void SetComponentTables(ComponentTable<PhysicalComponent>* physical)
    {
        m_physical_components = physical;
    }

    void RenderScene(const Viewport* viewport, uint camera, const Scene* scene)
    {
        SetActiveCamera(camera);
        m_renderer->RenderScene(viewport, &m_camera, scene);
    }

private:
    void SetActiveCamera(uint camera)
    {
        auto *physical = m_physical_components->GetComponent(camera);
        m_camera.CalcView(physical->forward, physical->position, physical->up);
    }

private:
    Renderer3D *m_renderer;
    ComponentTable<PhysicalComponent> *m_physical_components;

    Camera m_camera;
};
