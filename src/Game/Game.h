
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
            physical.orientation = glm::quat();
            m_entity_system.AttachComponent(m_camera, &physical);
        }

        m_jiggy = m_entity_system.CreateEntity();
        {
            PhysicalComponent physical;
            physical.position = glm::vec3(0.0f, -20.0f, 150.0f);
            physical.orientation = glm::quat();
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

        int pos = -24;
        for (uint &note : m_notes)
        {
            note = m_entity_system.CreateEntity();

            PhysicalComponent physical;
            physical.position = glm::vec3((float)pos, 0.0f, 50.0f);
            m_entity_system.AttachComponent(note, &physical);

            GraphicalComponent graphical;
            graphical.mesh = LoadMeshFromOBJ("../models/note.obj");
            m_entity_system.AttachComponent(note, &graphical);
            pos += 16;
        }
    }

    void Shutdown()
    {
        for (uint note : m_notes)
        {
            m_entity_system.DestroyEntity(note);
        }
        m_entity_system.DestroyEntity(m_jiggy);
        m_entity_system.DestroyEntity(m_camera);
        m_physics->Cleanup();
        m_entity_system.CommitChanges();
    }

    void Simulate(uint64 tick, uint32 dt)
    {
        auto *physical_table = m_entity_system.GetTable<PhysicalComponent>();

        float angle = glm::radians(135.0f * ((float)dt / 1000.0f));
        float amount = glm::sin(tick / 200.0f) * 0.05f;

        auto *camera_physical = physical_table->GetComponent(m_camera);
        camera_physical->orientation = glm::rotate(camera_physical->orientation, glm::radians(amount*3.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        for (uint note : m_notes)
        {
            auto *note_physical = physical_table->GetComponent(note);
            note_physical->orientation = glm::rotate(note_physical->orientation, angle, glm::vec3(0.0f, 1.0f, 0.0f));
            note_physical->position += glm::vec3(0.0f, amount, 0.0f);
        }
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
    uint m_notes[4];
};
