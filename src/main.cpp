
#include <iostream>

#include "Application/Application.h"

#include "World.h"

#include "Renderer/Renderer.h"
#include "Renderer/3DRenderer/3DRenderer.h"
#include "Renderer/3DRenderer/Camera.h"
#include "Renderer/3DRenderer/Renderable.h"
#include "Renderer/3DRenderer/Scene.h"
#include "Renderer/3DRenderer/ModelParser_OBJ.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "Input/InputContext.h"
#include "Input/InputMapper.h"

#include "EntitySystem/ComponentTable.h"
#include "EntitySystem/PhysicalComponent.h"
#include "EntitySystem/RenderProxy.h"

static const float vertex_data[] = 
{
    -1.0f, -1.0f, -1.0f, // triangle 1 : begin
    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f, -1.0f, // triangle 2 : begin
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f, // triangle 2 : end
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f
};

static const float colour_data[] =
{
    0.583f, 0.771f, 0.014f,
    0.609f, 0.115f, 0.436f,
    0.327f, 0.483f, 0.844f,
    0.822f, 0.569f, 0.201f,
    0.435f, 0.602f, 0.223f,
    0.310f, 0.747f, 0.185f,
    0.597f, 0.770f, 0.761f,
    0.559f, 0.436f, 0.730f,
    0.359f, 0.583f, 0.152f,
    0.483f, 0.596f, 0.789f,
    0.559f, 0.861f, 0.639f,
    0.195f, 0.548f, 0.859f,
    0.014f, 0.184f, 0.576f,
    0.771f, 0.328f, 0.970f,
    0.406f, 0.615f, 0.116f,
    0.676f, 0.977f, 0.133f,
    0.971f, 0.572f, 0.833f,
    0.140f, 0.616f, 0.489f,
    0.997f, 0.513f, 0.064f,
    0.945f, 0.719f, 0.592f,
    0.543f, 0.021f, 0.978f,
    0.279f, 0.317f, 0.505f,
    0.167f, 0.620f, 0.077f,
    0.347f, 0.857f, 0.137f,
    0.055f, 0.953f, 0.042f,
    0.714f, 0.505f, 0.345f,
    0.783f, 0.290f, 0.734f,
    0.722f, 0.645f, 0.174f,
    0.302f, 0.455f, 0.848f,
    0.225f, 0.587f, 0.040f,
    0.517f, 0.713f, 0.338f,
    0.053f, 0.959f, 0.120f,
    0.393f, 0.621f, 0.362f,
    0.673f, 0.211f, 0.457f,
    0.820f, 0.883f, 0.371f,
    0.982f, 0.099f, 0.879f
};

static const float tri_data[] =
{
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f
};

static const float tri_colour[] =
{
    0.583f, 0.771f, 0.014f,
    0.609f, 0.115f, 0.436f,
    0.327f, 0.483f, 0.844f
};

static const float tri_uv[] =
{
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f
};

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
    render_proxy.SetComponentTables(&physical_components);

    uint camera = 0;
    {
        auto* physical = physical_components.AttachComponent(camera);
        physical->position = glm::vec3(0.0f, -20.0f, -150.0f);
        physical->forward = glm::vec3(0.0f, 0.0f, 1.0f);
        physical->up = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    auto box_transform = glm::translate(glm::vec3(-3.0f, 0.0f, 0.0f))
        * glm::rotate(30.0f, glm::vec3(0, 0, 1.0f))
        * glm::scale(glm::vec3(1.0f));

    Renderable box;
    box.SetTransform(box_transform);
    {
        auto mesh = box.GetMesh();
        mesh->LoadVerticies(vertex_data,
                            3 * sizeof(float),
                            sizeof(vertex_data) / 3 / sizeof(vertex_data[0]));
    }

    auto verts = ParseOBJ("../models/jiggy.obj");

    Renderable tri;
    tri.SetTransform(glm::translate(glm::vec3(0.0f, -20.0f, -150.0f)));
    {
        auto mesh = tri.GetMesh();
        mesh->LoadVerticies(verts.data(),
                            sizeof(glm::vec3),
                            verts.size());
    }

    Scene scene;
    scene.AddObject(&box);
    scene.AddObject(&tri);

    KeyboardContext k;
    k.AddMapping({ SDL_SCANCODE_A }, &::callback);
    k.AddMapping({ SDL_SCANCODE_S }, &::callback);
    k.AddMapping({ SDL_SCANCODE_D }, &::callback);
    k.AddMapping({ SDL_SCANCODE_W }, &::callback);
    k.AddMapping({ SDL_SCANCODE_LCTRL, SDL_SCANCODE_X }, &::callback);

    InputMapper input;
    input.LoadContext(std::move(k));

    //using namespace std::placeholders;
    //sdl.RegisterKeyboardEventListener(std::bind(&InputMapper::ReceiveInput, &input, _1));

    World w;
    w.Initialize();

    const uint SERVER_FRAME_DT = 10000;

    while (ApplicationService::FlushAndRefreshEvents(),
          !ApplicationService::QuitRequested())
    {
        w.Update(SERVER_FRAME_DT);
        input.DispatchCallbacks();
        render_proxy.RenderScene(&viewport1, camera, &scene);
        render_proxy.RenderScene(&viewport2, camera, &scene);
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
