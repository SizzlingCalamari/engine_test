
#pragma once

#include "../EntitySystem/EntitySystem.h"
#include "CameraController_ThirdPerson.h"
#include "CameraController_InterpolatedPath.h"
#include "../Input/InputContext.h"
#include "BillboardController.h"
#include "Thwomp.h"

class RenderProxy;
class PhysicsProxy;
class InputMapper;
struct PhysicalComponent;

namespace SDLAudio
{
    class AudioDevice;
}

struct EngineContext
{
    RenderProxy *renderer;
    PhysicsProxy *physics;
    InputMapper *input;
    SDLAudio::AudioDevice* audio;
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

    void CameraSimulation(uint32 dt);
    static bool HandleCameraMovement(PhysicalComponent *camera, uint32 dt);

    void LoadResources();
    void LoadEnts();

    void LoadFur();

private:
    EntitySystem m_entity_system;

    RenderProxy *m_renderer;
    PhysicsProxy *m_physics;
    SDLAudio::AudioDevice* m_audio;

    Uint8* m_jesusThwompBuf;
    Uint32 m_jesusThwompLen;

    uint m_roomMesh;
    uint m_floorMesh;
    uint m_teapotMesh;
    uint m_pedestalMesh;
    uint m_billboardMesh;

    uint m_woodFloorMaterial;
    uint m_jesusMaterial;
    uint m_teapotDefaultMaterial;
    uint m_teapotMarbleMaterial;
    uint m_teapotBumpMaterial;
    uint m_teapotCelMaterial;
    uint m_wallMaterial;
    uint m_pedestalMaterial;

    uint m_moonLightDirectional;

    uint m_camera;
    uint m_roomEnt;
    uint m_floorEnt;

    uint m_pedestalTeapot1;
    uint m_teapotBumpedEnt;

    uint m_pedestalTeapot2;
    uint m_teapotMarbleEnt;

    uint m_pedestalTeapot3;
    uint m_teapotCelEnt;

    uint m_moonLightEnt;
    
    uint m_billboardTextMaterials[3];
    uint m_billboardTextEnts[3];

    uint m_painting100;

    uint m_paintingMaterials[6];
    uint m_paintingEnts[6];

    uint m_spotLights[9];
    uint m_spotLightEnts[9];

    CameraController_ThirdPerson m_thirdperson_controller;
    CameraController_InterpolatedPath m_cameraPathController;
    BillboardController m_billboardController;

    uint m_inputIdZ;
    uint m_inputIdP;
    uint m_inputIdX;

    enum CameraType
    {
        Camera_ThirdPerson = 0,
        Camera_FirstPerson,
        Camera_InterpolatedPath
    };
    CameraType m_activeCameraType;

    Thwomp m_thwomp;

    uint m_thwompEnt;
    uint m_thwompMesh;
    uint m_thwompMat;

    uint m_furMesh;
    uint m_furMats[4];
    uint m_furEnts[7];
};
