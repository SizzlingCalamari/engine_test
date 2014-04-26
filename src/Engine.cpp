
#pragma once

#include "Engine.h"
#include "SDL_timer.h"
#include "Application/Application.h"

#include "Renderer/Renderer.h"
#include "Renderer/3DRenderer/3DRenderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "EntitySystem/EntitySystem.h"

void Engine::Initialize()
{
    m_window = ApplicationService::CreateWindow(
        "JORDAN", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);
    m_gl_context = ApplicationService::CreateGLContext(m_window);
    m_renderer = Renderer::CreateRenderer3D(m_gl_context);

    renderer3d_config config;
    config.x = 0;
    config.y = 0;
    config.width = 640;
    config.height = 480;
    m_renderer->Init(config);
    m_render_proxy = RenderProxy(m_renderer);

    m_dynamics_world.Initialize();
    m_physics_proxy = PhysicsProxy(&m_dynamics_world);

    EngineContext engine;
    engine.renderer = &m_render_proxy;
    engine.physics = &m_physics_proxy;

    m_game.Initialize(engine);
}

void Engine::Shutdown()
{
    m_game.Shutdown();

    m_dynamics_world.Shutdown();

    m_renderer->Shutdown();
    Renderer::FreeRenderer(m_renderer);

    ApplicationService::FreeGLContext(m_gl_context);
    ApplicationService::FreeWindow(m_window);
}

void Engine::Run()
{
    uint64 cur_tick = 0;
    const uint32 dt = 10;

    uint32 currentTime = SDL_GetTicks();
    uint32 accumulator = 0;

    while (ApplicationService::FlushAndRefreshEvents(),
            !ApplicationService::QuitRequested())
    {
        uint32 newTime = SDL_GetTicks();
        uint32 frameTime = newTime - currentTime;
        if (frameTime > 250)
        {
            frameTime = 250;
        }
        currentTime = newTime;

        accumulator += frameTime;

        while (accumulator >= dt)
        {
            Simulate(cur_tick, dt);
            cur_tick += dt;
            accumulator -= dt;
        }

        Render();
    }
}

void Engine::Simulate(uint64 tick, uint32 dt)
{
    //input.DispatchCallbacks();
    m_game.Simulate(tick, dt);
}

void Engine::Render()
{
    m_game.Render();
    m_window.SwapBuffers();
}
