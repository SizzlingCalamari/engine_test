
#include <iostream>

#include "Application/Application.h"

#include "Renderer/Renderer.h"
#include "Renderer/3DRenderer/3DRenderer.h"
#include "Renderer/3DRenderer/Camera.h"
#include "Renderer/3DRenderer/Renderable.h"
#include "Renderer/3DRenderer/ModelParser_OBJ.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "Input/InputContext.h"
#include "Input/InputMapper.h"

#include "Physics/DynamicsWorld.h"

#include "EntitySystem/ComponentTable.h"
#include "EntitySystem/GraphicalComponent.h"
#include "EntitySystem/PhysicalComponent.h"
#include "EntitySystem/DynamicsComponent.h"
#include "EntitySystem/RenderProxy.h"
#include "EntitySystem/PhysicsProxy.h"

#include "EntitySystem/EntitySystem.h"

void callback(const KeyboardEventInfo& info)
{
    ButtonCombination b = info.buttons;
    if (b.m_buttons[0])
    {
        std::cout << b.m_buttons[0] << " ";
    }
    if (info.buttons.m_buttons[1])
    {
        std::cout << b.m_buttons[1] << " ";
    }
    if (info.buttons.m_buttons[2])
    {
        std::cout << b.m_buttons[2] << " ";
    }
    if (info.buttons.m_buttons[3])
    {
        std::cout << b.m_buttons[3] << " ";
    }

    if (info.pressed)
    {
        std::cout << "down";
    }
    else
    {
        std::cout << "up";
    }
    std::cout << std::endl;
}

int main(int argc, const char *argv[])
{
    ApplicationService::Initialize();

    Window window = ApplicationService::CreateWindow(
        "JORDAN", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);
    auto gl_context = ApplicationService::CreateGLContext(window);
    auto renderer = Renderer::CreateRenderer3D(gl_context);

    renderer3d_config config;
    config.x = 0;
    config.y = 0;
    config.width = 640;
    config.height = 480;
    renderer->Init(config);

    RenderProxy render_proxy(renderer);

    Viewport viewport1(0, 0, 320, 240);
    Viewport viewport2(320, 240, 320, 240);

    EntitySystem entity_system;

    render_proxy.SetComponentTables(entity_system.GetTable<PhysicalComponent>(),
                                    entity_system.GetTable<GraphicalComponent>());
    std::vector<uint> renderables;

    DynamicsWorld dynamics_world;
    dynamics_world.Initialize();

    PhysicsProxy physics_proxy(&dynamics_world);
    physics_proxy.SetComponentTables(entity_system.GetTable<PhysicalComponent>(),
                                     entity_system.GetTable<DynamicsComponent>());

    uint camera = 0;
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(0.0f, 0.0f, 0.0f);
        physical.forward = glm::vec3(0.0f, 0.0f, 1.0f);
        physical.up = glm::vec3(0.0f, 1.0f, 0.0f);
        entity_system.AttachComponent(camera, &physical);
    }

    uint jiggy = 1;
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(0.0f, -20.0f, 150.0f);
        physical.forward = glm::vec3(0.0f, 0.0f, -1.0f);
        physical.up = glm::vec3(0.0f, 1.0f, 0.0f);
        entity_system.AttachComponent(jiggy, &physical);

        GraphicalComponent graphical;
        graphical.mesh = LoadMeshFromOBJ("../models/jiggy.obj");
        entity_system.AttachComponent(jiggy, &graphical);
        renderables.emplace_back(jiggy);

        DynamicsComponent dynamics;
        dynamics.mass = 1.0f;
        dynamics.inertia = glm::vec3(0.0f);
        dynamics.shape = new btBoxShape(btVector3{ 3.0f, 3.0f, 1.0f });
        entity_system.AttachComponent(jiggy, &dynamics);
    }

    uint note = 2;
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(5.0f, 0.0f, 50.0f);
        physical.forward = glm::vec3(0.0f, 0.0f, -1.0f);
        physical.up = glm::vec3(0.0f, 1.0f, 0.0f);
        entity_system.AttachComponent(note, &physical);

        GraphicalComponent graphical;
        graphical.mesh = LoadMeshFromOBJ("../models/note.obj");
        entity_system.AttachComponent(note, &graphical);
        renderables.emplace_back(note);
    }

    uint camera2 = 3;
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(0.0f, -10.0f, 300.0f);
        physical.forward = glm::vec3(0.0f, 0.0f, 1.0f);
        physical.up = glm::vec3(0.0f, 1.0f, 0.0f);
        entity_system.AttachComponent(camera2, &physical);
    }

    KeyboardContext k;
    k.AddMapping({ SDL_SCANCODE_A }, &::callback);
    k.AddMapping({ SDL_SCANCODE_S }, &::callback);
    k.AddMapping({ SDL_SCANCODE_D }, &::callback);
    k.AddMapping({ SDL_SCANCODE_W }, &::callback);
    k.AddMapping({ SDL_SCANCODE_LCTRL, SDL_SCANCODE_X }, &::callback);

    InputMapper input;
    input.LoadContext(std::move(k));

    const uint SERVER_FRAME_DT = 10000;
    const float dt = 1.0f / 60.0f;

    while (ApplicationService::FlushAndRefreshEvents(),
          !ApplicationService::QuitRequested())
    {
        input.DispatchCallbacks();

        physics_proxy.Simulate(dt);
        render_proxy.Update();

        render_proxy.RenderScene(&viewport1, camera);
        render_proxy.RenderScene(&viewport2, jiggy);
        window.SwapBuffers();

        entity_system.CommitChanges();
    }

    dynamics_world.Shutdown();
    renderer->Shutdown();
    Renderer::FreeRenderer(renderer);
    ApplicationService::FreeGLContext(gl_context);
    ApplicationService::FreeWindow(window);
    ApplicationService::Shutdown();
    return 0;
}
