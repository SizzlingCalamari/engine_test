
#include "Engine.h"
#include "SDL_timer.h"
#include "Application/Application.h"

#include "Base/Time.h"

#include "Renderer/Renderer.h"
#include "Renderer/3DRenderer/3DRenderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "EntitySystem/EntitySystem.h"

#include "Audio/public/hAudioDevice.h"

void Engine::Initialize()
{
    renderer3d_config config;
    config.x = 0;
    config.y = 0;
    config.width = 1280;
    config.height = 720;

    m_window = ApplicationService::CreateWindow(
        "JORDAN", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        config.width, config.height, SDL_WINDOW_OPENGL);
    m_gl_context = ApplicationService::CreateGLContext(m_window);
    m_renderer = Renderer::CreateRenderer3D(m_gl_context);

    m_renderer->Init(config);
    m_render_proxy = new RenderProxy(m_renderer);

    m_dynamics_world.Initialize();
    m_physics_proxy = PhysicsProxy(&m_dynamics_world);

    m_audioSystem = SDLAudio::CreateAudioSystem();

    SDLAudio::AudioDeviceDesc desc{};
    desc.deviceName = nullptr;
    desc.audioSpec.freq = 44100;
    desc.audioSpec.format = AUDIO_S16;
    desc.audioSpec.channels = 1;
    desc.audioSpec.samples = 2048;
    desc.audioSpec.callback = nullptr;
    desc.audioSpec.userdata = nullptr;

    SDLAudio::AudioDevice* audioDevice = m_audioSystem->OpenAudioDevice(desc);
    if(audioDevice)
    {
        printf("Audio Driver: %s\n", m_audioSystem->GetCurrentAudioDriver());
        printf("Audio Device: %s\n", audioDevice->GetDeviceDesc().deviceName);
        audioDevice->StartPlayback();
    }

    EngineContext engine;
    engine.renderer = m_render_proxy;
    engine.physics = &m_physics_proxy;
    engine.input = &m_inputMapper;
    engine.audio = audioDevice;

    m_game.Initialize(engine);

    // Allows the game to have simulate run
    // at least once before it shuts down
    m_game.Simulate(0, 0);
}

void Engine::Shutdown()
{
    m_game.Shutdown();

    SDLAudio::FreeAudioSystem(m_audioSystem);

    m_dynamics_world.Shutdown();

    delete m_render_proxy;
    m_render_proxy = nullptr;

    m_renderer->Shutdown();
    Renderer::FreeRenderer(m_renderer);

    ApplicationService::FreeGLContext(m_gl_context);
    ApplicationService::FreeWindow(m_window);
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

    while (ApplicationService::FlushAndRefreshEvents(),
            !ApplicationService::QuitRequested())
    {
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
    }
}

void Engine::Simulate(uint64 tick, uint32 dt)
{
    m_game.Simulate(tick, dt);
}

void Engine::Render()
{
    m_game.Render();
    m_window.SwapBuffers();
}
