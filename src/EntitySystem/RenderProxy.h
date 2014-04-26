
#pragma once

#include "../Renderer/3DRenderer/Camera.h"
#include "../Renderer/3DRenderer/Renderable.h"
#include "ComponentTable.h"
#include "PhysicalComponent.h"
#include "GraphicalComponent.h"
#include <vector>

class Renderer3D;
class Viewport;

class RenderProxy
{
public:
    RenderProxy(Renderer3D* renderer = nullptr);

    void SetComponentTables(ComponentTable<PhysicalComponent>* physical,
                            ComponentTable<GraphicalComponent>* graphical);

    void RenderScene(const Viewport* viewport, uint camera);

    void Update();

private:
    void SetActiveCamera(uint camera);

    void BuildScene();

private:
    Renderer3D *m_renderer;
    ComponentTable<PhysicalComponent> *m_physical_components;
    ComponentTable<GraphicalComponent> *m_graphical_components;

    Camera m_camera;
    std::vector<Renderable> m_scene;
};
