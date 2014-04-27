
#include "Game.h"

#include "../EntitySystem/RenderProxy.h"
#include "../EntitySystem/PhysicsProxy.h"

#include "../EntitySystem/PhysicalComponent.h"
#include "../EntitySystem/GraphicalComponent.h"

#include "../Renderer/3DRenderer/ModelParser_OBJ.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "mathutils.h"

#include "SDL_keyboard.h"
#include "SDL_mouse.h"

void Game::Initialize(const EngineContext& engine)
{
    m_renderer = engine.renderer;
    m_physics = engine.physics;

    m_renderer->SetComponentTables(m_entity_system.GetTable<PhysicalComponent>(),
                                   m_entity_system.GetTable<GraphicalComponent>());

    m_physics->SetComponentTables(m_entity_system.GetTable<PhysicalComponent>(),
                                  m_entity_system.GetTable<DynamicsComponent>());

    m_thirdperson_controller.SetComponentTables(m_entity_system.GetTable<PhysicalComponent>());

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

    m_thirdperson_controller.SetCameraEnt(m_camera);
    m_thirdperson_controller.SetTargetEnt(m_notes[1]);
    m_thirdperson_controller.SetRadiusFromTarget(75.0f);
}

void Game::Shutdown()
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

void Game::Simulate(uint64 tick, uint32 dt)
{
    auto *physical_table = m_entity_system.GetTable<PhysicalComponent>();

    float angle = glm::radians(135.0f * ((float)dt / 1000.0f));
    float amount = glm::sin(tick / 200.0f) * 0.05f;

    auto *camera_physical = physical_table->GetComponent(m_camera);
    //HandleCameraMovement(camera_physical, dt);
    m_thirdperson_controller.Update(dt);

    for (uint note : m_notes)
    {
        auto *note_physical = physical_table->GetComponent(note);
        note_physical->orientation = math::rotate_local(note_physical->orientation, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        note_physical->position += glm::vec3(0.0f, amount, 0.0f);
    }
    m_physics->Simulate(dt);

    m_entity_system.CommitChanges();
}

void Game::Render()
{
    m_renderer->Update();
    m_renderer->RenderScene(nullptr, m_camera);
}

void Game::HandleCameraMovement(PhysicalComponent *camera, uint32 dt)
{
    // keyboard camera handling
    {
        float key_factor = float(dt * 0.1);
        auto *keys = SDL_GetKeyboardState(nullptr);
            
        if (!keys[SDL_SCANCODE_W] || !keys[SDL_SCANCODE_S])
        {
            auto forward = math::forward(camera->orientation);
            if (keys[SDL_SCANCODE_W])
            {
                camera->position += (forward * key_factor);
            }
            else if (keys[SDL_SCANCODE_S])
            {
                camera->position -= (forward * key_factor);
            }
        }
        if (!keys[SDL_SCANCODE_A] || !keys[SDL_SCANCODE_D])
        {
            auto right = math::right(camera->orientation);
            if (keys[SDL_SCANCODE_A])
            {
                camera->position += (right * key_factor);
            }
            else if (keys[SDL_SCANCODE_D])
            {
                camera->position -= (right * key_factor);
            }
        }
    }

    // mouse camera handling
    {
        float mouse_factor = glm::radians(dt * 0.02f);
        int x = 0;
        int y = 0;
        auto mouse = SDL_GetRelativeMouseState(&x, &y);

        if (mouse & SDL_BUTTON(SDL_BUTTON_RIGHT) && (x != 0 || y != 0))
        {
            if (x != 0)
            {
                // left/right rotations. no angle clamping
                const auto y_axis = glm::vec3(0.0f, 1.0f, 0.0f);
                camera->orientation = math::rotate_world(camera->orientation, -x*mouse_factor, y_axis);
            }

            if (y != 0)
            {
                // up/down rotations. clamp to +-90 degrees.
                // A better idea would be to limit the rotation before
                // the rotation calculation, but learning the quaternion math was fun.
                const auto x_axis = glm::vec3(1.0f, 0.0f, 0.0f);
                auto after_rotation = math::rotate_local(camera->orientation, y*mouse_factor, x_axis);
                camera->orientation = math::limit_rotation_xaxis(after_rotation, glm::half_pi<float>());
            }
        }
    }
}
