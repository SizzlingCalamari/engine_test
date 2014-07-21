
#pragma once

#include "../EntitySystem/EntitySystem.h"
#include "CameraController_ThirdPerson.h"
#include "CameraController_InterpolatedPath.h"

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

    static bool HandleCameraMovement(PhysicalComponent *camera, uint32 dt);

private:
    EntitySystem m_entity_system;

    RenderProxy *m_renderer;
    PhysicsProxy *m_physics;

    uint m_camera;
    uint m_floor;
    uint m_humanoid;
    uint m_jiggy;
    uint m_notes[4];

    CameraController_ThirdPerson m_thirdperson_controller;
    CameraController_InterpolatedPath m_cameraPathController;
};
