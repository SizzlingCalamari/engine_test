
#pragma once

#include "../EntitySystem/EntitySystem.h"

class RenderProxy;
class PhysicsProxy;
struct PhysicalComponent;

struct EngineContext
{
    RenderProxy *renderer;
    PhysicsProxy *physics;
};

class Game
{
public:
    void Initialize(const EngineContext& engine);

    void Shutdown();

    void Simulate(uint64 tick, uint32 dt);

    void Render();

    static void HandleCameraMovement(PhysicalComponent *camera, uint32 dt);

private:
    EntitySystem m_entity_system;

    RenderProxy *m_renderer;
    PhysicsProxy *m_physics;

    uint m_camera;
    uint m_jiggy;
    uint m_notes[4];
};
