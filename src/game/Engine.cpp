
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

void Engine::Run()
{
    uint64 cur_tick = 0;
    const uint32 dt = 10;

    uint32 currentTime = SDL_GetTicks();
    uint32 accumulator = 0;

    const float renderPeriodSec = (1.0f / 60.0f);
    const Time::Duration renderPeriod = Time::FromSeconds(renderPeriodSec);
    
    Time::StopWatch renderWatch;
    Time::Duration renderAcc = renderPeriod;

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

        m_inputMapper.DispatchCallbacks();
        while (accumulator >= dt)
        {
            Simulate(cur_tick, dt);
            cur_tick += dt;
            accumulator -= dt;
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
