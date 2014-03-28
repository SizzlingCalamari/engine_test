
#include <iostream>

#include "Application/Application.h"

#include "World.h"

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

#include "EntitySystem/ComponentTable.h"
#include "EntitySystem/GraphicalComponent.h"
#include "EntitySystem/PhysicalComponent.h"
#include "EntitySystem/RenderProxy.h"

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

    ComponentTable<PhysicalComponent> physical_components;
    ComponentTable<GraphicalComponent> graphical_components;

    render_proxy.SetComponentTables(&physical_components, &graphical_components);
    std::vector<uint> renderables;

    uint camera = 0;
    {
        auto* physical = physical_components.AttachComponent(camera);
        physical->position = glm::vec3(0.0f, 0.0f, 0.0f);
        physical->forward = glm::vec3(0.0f, 0.0f, 1.0f);
        physical->up = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    uint jiggy = 1;
    {
        auto* physical = physical_components.AttachComponent(jiggy);
        physical->position = glm::vec3(0.0f, -20.0f, 150.0f);
        physical->forward = glm::vec3(0.0f, 0.0f, -1.0f);
        physical->up = glm::vec3(0.0f, 1.0f, 0.0f);

        auto* graphical = graphical_components.AttachComponent(jiggy);
        graphical->mesh = LoadMeshFromOBJ("../models/jiggy.obj");

        renderables.emplace_back(jiggy);
    }

    uint note = 2;
    {
        auto* physical = physical_components.AttachComponent(note);
        physical->position = glm::vec3(5.0f, 0.0f, 50.0f);
        physical->forward = glm::vec3(0.0f, 0.0f, -1.0f);
        physical->up = glm::vec3(0.0f, 1.0f, 0.0f);

        auto* graphical = graphical_components.AttachComponent(note);
        graphical->mesh = LoadMeshFromOBJ("../models/note.obj");

        renderables.emplace_back(note);
    }

    uint camera2 = 3;
    {
        auto* physical = physical_components.AttachComponent(camera2);
        physical->position = glm::vec3(0.0f, -10.0f, 300.0f);
        physical->forward = glm::vec3(0.0f, 0.0f, -1.0f);
        physical->up = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    KeyboardContext k;
    k.AddMapping({ SDL_SCANCODE_A }, &::callback);
    k.AddMapping({ SDL_SCANCODE_S }, &::callback);
    k.AddMapping({ SDL_SCANCODE_D }, &::callback);
    k.AddMapping({ SDL_SCANCODE_W }, &::callback);
    k.AddMapping({ SDL_SCANCODE_LCTRL, SDL_SCANCODE_X }, &::callback);

    InputMapper input;
    input.LoadContext(std::move(k));

    World w;
    w.Initialize();

    const uint SERVER_FRAME_DT = 10000;

    while (ApplicationService::FlushAndRefreshEvents(),
          !ApplicationService::QuitRequested())
    {
        w.Update(SERVER_FRAME_DT);
        input.DispatchCallbacks();
        render_proxy.RenderScene(&viewport1, camera, renderables);
        render_proxy.RenderScene(&viewport2, camera2, renderables);
        window.SwapBuffers();
    }

    w.Shutdown();
    renderer->Shutdown();
    Renderer::FreeRenderer(renderer);
    ApplicationService::FreeGLContext(gl_context);
    ApplicationService::FreeWindow(window);
    ApplicationService::Shutdown();
    return 0;
}
