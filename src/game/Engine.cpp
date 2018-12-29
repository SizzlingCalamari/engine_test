
#include "Engine.h"

#include "SDL_timer.h"
#include "SDL_video.h"
#include "SDL_events.h"
#include "SDL_audio.h"

#include "Base/Time.h"

#include "Renderer/Renderer.h"
#include "Renderer/3DRenderer/3DRenderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "EntitySystem/EntitySystem.h"

void Engine::Initialize()
{
    renderer3d_config config;
    config.x = 0;
    config.y = 0;
    config.width = 1280;
    config.height = 720;

    m_window = SDL_CreateWindow(
        "JORDAN", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        config.width, config.height, SDL_WINDOW_OPENGL);
    m_gl_context = SDL_GL_CreateContext(m_window);
    m_renderer = Renderer::CreateRenderer3D(m_gl_context);

    m_renderer->Init(config);
    m_render_proxy = new RenderProxy(m_renderer);

    m_dynamics_world.Initialize();
    m_physics_proxy = PhysicsProxy(&m_dynamics_world);

    SDL_AudioSpec audioSpec;
    audioSpec.freq = 44100;
    audioSpec.format = AUDIO_S16;
    audioSpec.channels = 1;
    //audioSpec.silence = 0;
    audioSpec.samples = 2048;
    //audioSpec.padding = 0;
    //audioSpec.size = 0;
    audioSpec.callback = nullptr;
    audioSpec.userdata = nullptr;

    const SDL_AudioDeviceID audioDeviceId = SDL_OpenAudioDevice(NULL, 0, &audioSpec, &audioSpec, 0);
    mAudioDeviceId = audioDeviceId;

    if(audioDeviceId >= 2)
    {
        printf("Audio Driver: %s\n", SDL_GetCurrentAudioDriver());
        printf("Audio Device: %s\n", "default");

        SDL_PauseAudioDevice(audioDeviceId, 0);
    }

    EngineContext engine;
    engine.renderer = m_render_proxy;
    engine.physics = &m_physics_proxy;
    engine.input = &m_inputMapper;

    m_game.Initialize(engine);

    // Allows the game to have simulate run
    // at least once before it shuts down
    m_game.Simulate(0, 0);
}

void Engine::Shutdown()
{
    m_game.Shutdown();

    SDL_CloseAudioDevice(mAudioDeviceId);

    m_dynamics_world.Shutdown();

    delete m_render_proxy;
    m_render_proxy = nullptr;

    m_renderer->Shutdown();
    Renderer::FreeRenderer(m_renderer);

    SDL_GL_DeleteContext(m_gl_context);
    SDL_DestroyWindow(m_window);
}

class SDLStopWatch
{
public:
    SDLStopWatch():
        mStartTick(SDL_GetTicks())
    {
    }

    uint32 Reset()
    {
        const uint32 now = SDL_GetTicks();
        const uint32 elapsed = (now - mStartTick);
        mStartTick = now;
        return(elapsed);
    }

private:
    uint32 mStartTick;
};

void Engine::Run()
{
    uint64 cur_tick = 0;

    const uint32 logicPeriod = 10;

    SDLStopWatch logicWatch;
    uint32 logicAcc = logicPeriod;

    const float renderPeriodSec = (1.0f / 60.0f);
    const Time::Duration renderPeriod = Time::FromSeconds(renderPeriodSec);
    
    Time::StopWatch renderWatch;
    Time::Duration renderAcc = renderPeriod;

    for(;;)
    {
        SDL_PumpEvents();
        if (SDL_HasEvent(SDL_QUIT) == SDL_TRUE)
        {
            break;
        }

        m_inputMapper.DispatchCallbacks();

        logicAcc += std::min(250u, logicWatch.Reset());
        while (logicAcc >= logicPeriod)
        {
            Simulate(cur_tick, logicPeriod);
            cur_tick += logicPeriod;
            logicAcc -= logicPeriod;
        }

        renderAcc += renderWatch.Reset();
        if (renderAcc >= renderPeriod)
        {
            Render();
            renderAcc %= renderPeriod;
        }

        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
    }
}

void Engine::Simulate(uint64 tick, uint32 dt)
{
    m_game.Simulate(tick, dt);
}

void Engine::Render()
{
    m_game.Render();
    SDL_GL_SwapWindow(m_window);
}
