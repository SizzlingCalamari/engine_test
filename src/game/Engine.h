
#pragma once

#include "Game/Game.h"
#include "EntitySystem/RenderProxy.h"
#include "Physics/DynamicsWorld.h"
#include "EntitySystem/PhysicsProxy.h"
#include "Input/InputMapper.h"

typedef struct SDL_Window SDL_Window;
typedef void *SDL_GLContext;
typedef Uint32 SDL_AudioDeviceID;

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

    SDL_Window* m_window;
    SDL_GLContext m_gl_context;

    Renderer3D *m_renderer;
    RenderProxy *m_render_proxy;

    DynamicsWorld m_dynamics_world;
    PhysicsProxy m_physics_proxy;

    InputMapper m_inputMapper;

    SDL_AudioDeviceID mAudioDeviceId;
};
