
#pragma once

#include "Game/Game.h"
#include "Application/Window.h"
#include "EntitySystem/RenderProxy.h"
#include "Physics/DynamicsWorld.h"
#include "EntitySystem/PhysicsProxy.h"

class Renderer3D;

class Engine
{
public:
    void Initialize();

    void Shutdown();

    void Run();

private:
    void Simulate(uint64 tick, uint32 dt);

    void Render();

private:
    Game m_game;

    Window m_window;
    void *m_gl_context;
    Renderer3D *m_renderer;
    RenderProxy *m_render_proxy;

    DynamicsWorld m_dynamics_world;
    PhysicsProxy m_physics_proxy;
};
