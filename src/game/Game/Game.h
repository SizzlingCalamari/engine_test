
#pragma once

#include "../EntitySystem/EntitySystem.h"
#include "CameraController_ThirdPerson.h"
#include "CameraController_InterpolatedPath.h"
#include "../Input/InputContext.h"

class RenderProxy;
class PhysicsProxy;
class InputMapper;
struct PhysicalComponent;

struct EngineContext
{
    RenderProxy *renderer;
    PhysicsProxy *physics;
    InputMapper *input;
};

class Game
{
public:
    Game();

    void Initialize(const EngineContext& engine);

    void Shutdown();

    void Simulate(uint64 tick, uint32 dt);

    void Render();

private:
    void InputEventCallback(const KeyboardEventInfo& info);

private:
    void CameraSimulation(uint32 dt);
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

    uint m_inputIdZ;

    enum CameraType
    {
        Camera_ThirdPerson = 0,
        Camera_FirstPerson,
        Camera_InterpolatedPath
    };
    CameraType m_activeCameraType;
};
