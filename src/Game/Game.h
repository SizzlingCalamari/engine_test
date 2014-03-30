
#pragma once

#include "../EntitySystem/EntitySystem.h"
#include "../EntitySystem/PhysicalComponent.h"
#include "../EntitySystem/RenderProxy.h"
#include "../EntitySystem/PhysicsProxy.h"

struct EngineContext
{
    RenderProxy *renderer;
    PhysicsProxy *physics;
};

class Game
{
public:
    void Initialize(const EngineContext& engine)
    {
        m_renderer = engine.renderer;
        m_physics = engine.physics;

        m_renderer->SetComponentTables(m_entity_system.GetTable<PhysicalComponent>(),
                                       m_entity_system.GetTable<GraphicalComponent>());

        m_physics->SetComponentTables(m_entity_system.GetTable<PhysicalComponent>(),
                                      m_entity_system.GetTable<DynamicsComponent>());

        m_camera = m_entity_system.CreateEntity();
        {
            PhysicalComponent physical;
            physical.position = glm::vec3(0.0f, 0.0f, 0.0f);
            physical.forward = glm::vec3(0.0f, 0.0f, 1.0f);
            physical.up = glm::vec3(0.0f, 1.0f, 0.0f);
            m_entity_system.AttachComponent(m_camera, &physical);
        }

        m_jiggy = m_entity_system.CreateEntity();
        {
            PhysicalComponent physical;
            physical.position = glm::vec3(0.0f, -20.0f, 150.0f);
            physical.forward = glm::vec3(0.0f, 0.0f, -1.0f);
            physical.up = glm::vec3(0.0f, 1.0f, 0.0f);
            m_entity_system.AttachComponent(m_jiggy, &physical);

            GraphicalComponent graphical;
            graphical.mesh = LoadMeshFromOBJ("../models/jiggy.obj");
            m_entity_system.AttachComponent(m_jiggy, &graphical);

            DynamicsComponent dynamics;
            dynamics.mass = 1.0f;
            dynamics.inertia = glm::vec3(0.0f);
            dynamics.shape = new btBoxShape(btVector3{ 3.0f, 3.0f, 1.0f });
            m_entity_system.AttachComponent(m_jiggy, &dynamics);
        }

        m_note = m_entity_system.CreateEntity();
        {
            PhysicalComponent physical;
            physical.position = glm::vec3(5.0f, 0.0f, 50.0f);
            physical.forward = glm::vec3(0.0f, 0.0f, -1.0f);
            physical.up = glm::vec3(0.0f, 1.0f, 0.0f);
            m_entity_system.AttachComponent(m_note, &physical);

            GraphicalComponent graphical;
            graphical.mesh = LoadMeshFromOBJ("../models/note.obj");
            m_entity_system.AttachComponent(m_note, &graphical);
        }
    }

    void Shutdown()
    {
        m_entity_system.DestroyEntity(m_note);
        m_entity_system.DestroyEntity(m_jiggy);
        m_entity_system.DestroyEntity(m_camera);
        m_physics->Cleanup();
        m_entity_system.CommitChanges();
    }

    void Simulate(uint64 tick, uint32 dt)
    {
        m_physics->Simulate(dt);

        m_entity_system.CommitChanges();
    }

    void Render()
    {
        m_renderer->Update();
        m_renderer->RenderScene(nullptr, m_camera);
    }

private:
    EntitySystem m_entity_system;

    RenderProxy *m_renderer;
    PhysicsProxy *m_physics;

    uint m_camera;
    uint m_jiggy;
    uint m_note;
};
