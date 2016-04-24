
#include "Game.h"

#include "../EntitySystem/RenderProxy.h"
#include "../EntitySystem/PhysicsProxy.h"

#include "../EntitySystem/PhysicalComponent.h"
#include "../EntitySystem/GraphicalComponent.h"

#include "../Input/InputMapper.h"

#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "mathutils.h"

#include "SDL_keyboard.h"
#include "SDL_mouse.h"

#include <iostream>

Game::Game():
    m_renderer(nullptr),
    m_physics(nullptr),
    m_floorMesh(0),
    m_jesusMaterial(0),
    m_teapotDefaultMaterial(0),
    m_teapotMarbleMaterial(0),
    m_teapotBumpMaterial(0),
    m_teapotCelMaterial(0),
    m_moonLightDirectional(0),
    m_teapotMesh(0),
    m_pedestalMesh(0),
    m_wallBumpMaterial(0),
    m_pedestalBumpMaterial(0),
    m_camera(0),
    m_floorEnt(0),
    m_moonLightEnt(0),
    m_billboardTextMaterials(),
    m_billboardTextEnts(),
    m_inputIdZ(0),
    m_inputIdP(0),
    m_inputIdX(0),
    m_activeCameraType(Camera_InterpolatedPath)
{
}

void Game::Initialize(const EngineContext& engine)
{
    using namespace std::placeholders;
    KeyboardContext keyboardContext;
    {
        KeyCombination temp = {};
        temp[0] = SDL_SCANCODE_Z;
        m_inputIdZ = keyboardContext.AddMapping(temp, std::bind(&Game::InputEventCallback, this, _1));
        temp[0] = SDL_SCANCODE_P;
        m_inputIdP = keyboardContext.AddMapping(temp, std::bind(&Game::InputEventCallback, this, _1));
        temp[0] = SDL_SCANCODE_X;
        m_inputIdX = keyboardContext.AddMapping(temp, std::bind(&Game::InputEventCallback, this, _1));
    }

    engine.input->LoadContext(std::move(keyboardContext));

    m_renderer = engine.renderer;
    m_physics = engine.physics;

    m_renderer->SetComponentTables(m_entity_system.GetTable<PhysicalComponent>(),
                                   m_entity_system.GetTable<GraphicalComponent>());

    m_physics->SetComponentTables(m_entity_system.GetTable<PhysicalComponent>(),
                                  m_entity_system.GetTable<DynamicsComponent>());

    m_thirdperson_controller.SetComponentTables(m_entity_system.GetTable<PhysicalComponent>());
    m_cameraPathController.SetComponentTables(m_entity_system.GetTable<PhysicalComponent>());
    m_billboardController.SetComponentTables(m_entity_system.GetTable<PhysicalComponent>());

    LoadResources();
    LoadEnts();

    m_thirdperson_controller.SetCameraEnt(m_camera);
    m_thirdperson_controller.SetTargetEnt(m_teapotMarbleEnt);
    m_thirdperson_controller.SetRadiusFromTarget(200.0f);
    
    m_cameraPathController.SetCameraEnt(m_camera);
    m_cameraPathController.SetTimePerControlPoint(5000);
    m_cameraPathController.SetShouldLoop(false);
    m_cameraPathController.SetStartTime(0);

    m_cameraPathController.AddControlPoint(glm::vec3{ -700.0f, 600.0f, 0.0f }, glm::quat(glm::vec3{ 0.0f, glm::pi<float>(), 0.0f }));

    // teapots
    m_cameraPathController.AddControlPoint(glm::vec3{ -400.0f, 400.0f, -270.0f }, glm::quat(glm::vec3{ 0.0f, glm::pi<float>(), 0.0f }));
    m_cameraPathController.AddControlPoint(glm::vec3{ 0.0f, 400.0f, -270.0f }, glm::quat(glm::vec3{ 0.0f, glm::pi<float>(), 0.0f }));
    m_cameraPathController.AddControlPoint(glm::vec3{ 400.0f, 400.0f, -270.0f }, glm::quat(glm::vec3{ 0.0f, glm::pi<float>(), 0.0f }));

    // top corner shadows
    m_cameraPathController.AddControlPoint(glm::vec3{ 940.0f, 425.0f, 442.0f }, glm::quat(glm::vec3{ 0.0f, -3.0f*glm::quarter_pi<float>(), 0.0f }));

    // bert
    m_cameraPathController.AddControlPoint(glm::vec3{ 570.0f, 430.0f, 570.0f }, glm::quat(glm::vec3{ 0.0f, -1.25f*glm::half_pi<float>(), 0.0f }));

    // more teapots
    m_cameraPathController.AddControlPoint(glm::vec3{ 150.0f, 440.0f, 0.0f }, glm::quat(glm::vec3{ 0.0f, glm::pi<float>(), 0.0f }));

    // teapot view
    m_cameraPathController.AddControlPoint(glm::vec3{ -680.0f, 770.0f, 320.0f }, glm::quat(glm::vec3{ 0.5f*glm::quarter_pi<float>(), -1.25f*glm::pi<float>(), 0.0f }));

    // room view
    m_cameraPathController.AddControlPoint(glm::vec3{ -1440.0f, 1065.0f, -180.0f }, glm::quat(glm::vec3{ 0.5f*glm::quarter_pi<float>(), 1.10f*glm::half_pi<float>(), 0.0f }));

    // next corner room view
    m_cameraPathController.AddControlPoint(glm::vec3{ -1333.0f, 1033.0f, -1500.0f }, glm::quat(glm::vec3{ 0.5f*glm::quarter_pi<float>(), glm::quarter_pi<float>(), 0.0f }));

    // painting close
    m_cameraPathController.AddControlPoint(glm::vec3{ -700.0f, 575.0f, -1400.0f }, glm::quat(glm::vec3{ 0.5f*glm::quarter_pi<float>(), glm::quarter_pi<float>(), 0.0f }));

    // gg painting
    m_cameraPathController.AddControlPoint(glm::vec3{ -470.0f, 430.0f, -1276.0f }, glm::quat(glm::vec3{ 0.0f, 0.0f, 0.0f }));

    // next corner overview
    m_cameraPathController.AddControlPoint(glm::vec3{ 500.0f, 1024.0f, -1900.0f }, glm::quat(glm::vec3{ 0.5f*glm::quarter_pi<float>(), -0.5f*glm::quarter_pi<float>(), 0.0f }));

    // bob omb view
    m_cameraPathController.AddControlPoint(glm::vec3{ 1550.0f, 455.0f, -956.0f }, glm::quat(glm::vec3{ 0.0f, -glm::quarter_pi<float>(), 0.0f }));

    // teapots view again
    m_cameraPathController.AddControlPoint(glm::vec3{ 907.0f, 624.0f, 192.0f }, glm::quat(glm::vec3{ 0.5f*glm::quarter_pi<float>(), -1.5f*glm::half_pi<float>(), 0.0f }));

    m_billboardController.SetCameraEnt(m_camera);
    {
        CylindricalBillboard billboard;
        billboard.initialDirection = glm::vec3(0.0f, 0.0f, 1.0f);
        // 30 degrees
        billboard.maxAngleFromForward = glm::half_pi<float>() / 3.0f;
        m_billboardController.AddBillboard(m_billboardTextEnts[0], billboard);
        m_billboardController.AddBillboard(m_billboardTextEnts[1], billboard);
        m_billboardController.AddBillboard(m_billboardTextEnts[2], billboard);
    }
}

void Game::Shutdown()
{
    m_entity_system.DestroyEntity(m_camera);
    m_entity_system.DestroyEntity(m_floorEnt);

    m_physics->Cleanup();
    m_renderer->Update();
    m_entity_system.CommitChanges();
}

void Game::Simulate(uint64 tick, uint32 dt)
{
    CameraSimulation(dt);
    m_billboardController.Update();

    m_physics->Simulate(dt);
    m_renderer->Update();

    m_entity_system.CommitChanges();
}

void Game::Render()
{
    m_renderer->RenderScene(nullptr, m_camera);
}

void Game::InputEventCallback(const KeyboardEventInfo& info)
{
    if (info.mappingId == m_inputIdZ && info.pressed)
    {
        if (m_activeCameraType != Camera_InterpolatedPath)
        {
            m_activeCameraType = Camera_InterpolatedPath;
        }
        else
        {
            m_activeCameraType = Camera_FirstPerson;
        }
    }
    else if (info.mappingId == m_inputIdP && info.pressed)
    {
        auto* physical_table = m_entity_system.GetTable<PhysicalComponent>();
        auto* physical = physical_table->PeekComponent(m_camera);
        
        std::cout << physical->position.x
            << " " << physical->position.y
            << " " << physical->position.z << std::endl;
        auto forward = math::forward(physical->orientation);
        std::cout << forward.x << " " << forward.y << " " << forward.z << std::endl;
    }
    else if (info.mappingId == m_inputIdX && info.pressed)
    {
        m_activeCameraType = Camera_ThirdPerson;
    }
}

void Game::CameraSimulation(uint32 dt)
{
    auto *physical_table = m_entity_system.GetTable<PhysicalComponent>();
    switch (m_activeCameraType)
    {
    case Camera_FirstPerson:
        {
            auto camera_physical = physical_table->GetComponent(m_camera);
            if (HandleCameraMovement(&camera_physical, dt))
            {
                physical_table->EditComponent(m_camera, &camera_physical);
            }
        }
        break;
    case Camera_ThirdPerson:
        {
            const uint target = m_thirdperson_controller.GetTargetEnt();
            auto physical = physical_table->GetComponent(target);
            if (HandleCameraMovement(&physical, dt))
            {
                physical_table->EditComponent(target, &physical);
            }
            m_thirdperson_controller.Update(dt);
        }
        break;
    case Camera_InterpolatedPath:
        m_cameraPathController.Update(dt);
        break;
    default:
        break;
    }
}

bool Game::HandleCameraMovement(PhysicalComponent *camera, uint32 dt)
{
    bool updated = false;
    // keyboard camera handling
    {
        float key_factor = float(dt * 2.5);
        auto *keys = SDL_GetKeyboardState(nullptr);

        // Using != for bools is like an xor.
        // Only pass through if one of the keys is down and not both.
        if (keys[SDL_SCANCODE_W] != keys[SDL_SCANCODE_S])
        {
            updated = true;
            auto forward = math::forward(camera->orientation);
            if (keys[SDL_SCANCODE_W])
            {
                camera->position += (forward * key_factor);
            }
            else if (keys[SDL_SCANCODE_S])
            {
                camera->position -= (forward * key_factor);
            }
        }
        if (keys[SDL_SCANCODE_A] != keys[SDL_SCANCODE_D])
        {
            updated = true;
            auto right = math::right(camera->orientation);
            if (keys[SDL_SCANCODE_A])
            {
                camera->position += (right * key_factor);
            }
            else if (keys[SDL_SCANCODE_D])
            {
                camera->position -= (right * key_factor);
            }
        }
    }

    // mouse camera handling
    {
        float mouse_factor = glm::radians(dt * 0.02f);
        int x = 0;
        int y = 0;
        auto mouse = SDL_GetRelativeMouseState(&x, &y);

        if (mouse & SDL_BUTTON(SDL_BUTTON_RIGHT) && (x != 0 || y != 0))
        {
            updated = true;
            if (x != 0)
            {
                // left/right rotations. no angle clamping
                const auto y_axis = glm::vec3(0.0f, 1.0f, 0.0f);
                camera->orientation = math::rotate_world(camera->orientation, -x*mouse_factor, y_axis);
            }
            if (y != 0)
            {
                // up/down rotations. clamp to +-90 degrees.
                // A better idea would be to limit the rotation before
                // the rotation calculation, but learning the quaternion math was fun.
                const auto x_axis = glm::vec3(1.0f, 0.0f, 0.0f);
                auto after_rotation = math::rotate_local(camera->orientation, y*mouse_factor, x_axis);
                camera->orientation = math::limit_rotation_xaxis(after_rotation, glm::half_pi<float>());
            }
        }
    }
    return updated;
}

void Game::LoadResources()
{
    m_roomMesh = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::MeshObject;
        obj.properties.emplace("meshFile", "models/room.obj");
        m_roomMesh = m_renderer->CreateRenderObject(obj);
    }

    m_floorMesh = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::MeshObject;
        obj.properties.emplace("meshFile", "models/floor.obj");
        m_floorMesh = m_renderer->CreateRenderObject(obj);
    }

    m_teapotMesh = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::MeshObject;
        obj.properties.emplace("meshFile", "models/teapot/teapot.obj");
        m_teapotMesh = m_renderer->CreateRenderObject(obj);
    }

    m_pedestalMesh = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::MeshObject;
        obj.properties.emplace("meshFile", "models/pedestal.obj");
        m_pedestalMesh = m_renderer->CreateRenderObject(obj);
    }

    m_billboardMesh = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::MeshObject;
        obj.properties.emplace("meshFile", "models/billboard.obj");
        m_billboardMesh = m_renderer->CreateRenderObject(obj);
    }

    m_woodFloorMaterial = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::Material;
        obj.properties.emplace("diffuseMapFile", "textures/hardwood.jpg");
        obj.properties.emplace("specularIntensity", "1.0");
        obj.properties.emplace("specularPower", "32.0");
        m_woodFloorMaterial = m_renderer->CreateRenderObject(obj);
    }

    m_jesusMaterial = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::Material;
        obj.properties.emplace("diffuseMapFile", "textures/jesusbond_feelingfresh.jpg");
        obj.properties.emplace("specularIntensity", "1.0");
        obj.properties.emplace("specularPower", "32.0");
        m_jesusMaterial = m_renderer->CreateRenderObject(obj);
    }

    m_teapotDefaultMaterial = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::Material;
        obj.properties.emplace("diffuseMapFile", "models/teapot/default.png");
        obj.properties.emplace("specularIntensity", "1.0");
        obj.properties.emplace("specularPower", "32.0");
        m_teapotDefaultMaterial = m_renderer->CreateRenderObject(obj);
    }

    m_teapotMarbleMaterial = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::Material;
        obj.properties.emplace("specularIntensity", "1.0");
        obj.properties.emplace("specularPower", "32.0");
        obj.properties.emplace("noiseDiffuseMap", "1");
        m_teapotMarbleMaterial = m_renderer->CreateRenderObject(obj);
    }

    m_teapotBumpMaterial = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::Material;
        obj.properties.emplace("diffuseSolidColour", "0.5 0.5 0.5");
        obj.properties.emplace("specularIntensity", "1.0");
        obj.properties.emplace("specularPower", "32.0");
        obj.properties.emplace("noiseBumpMap", "1");
        m_teapotBumpMaterial = m_renderer->CreateRenderObject(obj);
    }

    m_teapotCelMaterial = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::Material;
        obj.properties.emplace("diffuseSolidColour", "0.1 0.7 0.1");
        obj.properties.emplace("specularIntensity", "1.0");
        obj.properties.emplace("specularPower", "32.0");
        obj.properties.emplace("celShaded", "1");
        m_teapotCelMaterial = m_renderer->CreateRenderObject(obj);
    }

    m_moonLightDirectional = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::DirectionalLight;
        obj.properties.emplace("colour", "1.0 1.0 1.0");
        obj.properties.emplace("ambientIntensity", "0.01");
        obj.properties.emplace("diffuseIntensity", "0.1");
        obj.properties.emplace("direction", "-0.5 -1.0 -1.0");
        m_moonLightDirectional = m_renderer->CreateRenderObject(obj);
    }

    m_wallBumpMaterial = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::Material;
        obj.properties.emplace("diffuseSolidColour", "0.5 0.5 0.5");
        obj.properties.emplace("specularIntensity", "1.0");
        obj.properties.emplace("specularPower", "32.0");
        obj.properties.emplace("noiseBumpMap", "1");
        m_wallBumpMaterial = m_renderer->CreateRenderObject(obj);
    }

    m_pedestalBumpMaterial = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::Material;
        obj.properties.emplace("diffuseSolidColour", "0.6 0.6 0.6");
        obj.properties.emplace("specularIntensity", "1.0");
        obj.properties.emplace("specularPower", "32.0");
        obj.properties.emplace("noiseBumpMap", "1");
        m_pedestalBumpMaterial = m_renderer->CreateRenderObject(obj);
    }

    // Billboard materials
    m_billboardTextMaterials[0] = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::Material;
        obj.properties.emplace("diffuseMapFile", "textures/bumpmapping.png");
        obj.properties.emplace("specularIntensity", "1.0");
        obj.properties.emplace("specularPower", "32.0");
        m_billboardTextMaterials[0] = m_renderer->CreateRenderObject(obj);
    }

    m_billboardTextMaterials[1] = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::Material;
        obj.properties.emplace("diffuseMapFile", "textures/fractalnoise.png");
        obj.properties.emplace("specularIntensity", "1.0");
        obj.properties.emplace("specularPower", "32.0");
        m_billboardTextMaterials[1] = m_renderer->CreateRenderObject(obj);
    }

    m_billboardTextMaterials[2] = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::Material;
        obj.properties.emplace("diffuseMapFile", "textures/celshading.png");
        obj.properties.emplace("specularIntensity", "1.0");
        obj.properties.emplace("specularPower", "32.0");
        m_billboardTextMaterials[2] = m_renderer->CreateRenderObject(obj);
    }

    // Painting mesh with aspect ratio 1.73
    m_painting173 = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::MeshObject;
        obj.properties.emplace("meshFile", "models/painting173.obj");
        m_painting173 = m_renderer->CreateRenderObject(obj);
    }

    m_painting100 = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::MeshObject;
        obj.properties.emplace("meshFile", "models/painting100.obj");
        m_painting100 = m_renderer->CreateRenderObject(obj);
    }

    m_paintint133 = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::MeshObject;
        obj.properties.emplace("meshFile", "models/painting133.obj");
        m_paintint133 = m_renderer->CreateRenderObject(obj);
    }

    // Painting Descriptions
    m_paintingMaterials[0] = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::Material;
        obj.properties.emplace("diffuseMapFile", "textures/mos98-02.png");
        obj.properties.emplace("specularIntensity", "1.0");
        obj.properties.emplace("specularPower", "32.0");
        m_paintingMaterials[0] = m_renderer->CreateRenderObject(obj);
    }

    m_paintingMaterials[1] = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::Material;
        obj.properties.emplace("diffuseMapFile", "textures/utah_teapot.jpg");
        obj.properties.emplace("specularIntensity", "1.0");
        obj.properties.emplace("specularPower", "32.0");
        m_paintingMaterials[1] = m_renderer->CreateRenderObject(obj);
    }

    m_paintingMaterials[2] = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::Material;
        obj.properties.emplace("diffuseMapFile", "textures/bob.jpg");
        obj.properties.emplace("specularIntensity", "1.0");
        obj.properties.emplace("specularPower", "32.0");
        m_paintingMaterials[2] = m_renderer->CreateRenderObject(obj);
    }

    m_paintingMaterials[3] = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::Material;
        obj.properties.emplace("diffuseMapFile", "textures/bertstare.jpg");
        obj.properties.emplace("specularIntensity", "1.0");
        obj.properties.emplace("specularPower", "32.0");
        m_paintingMaterials[3] = m_renderer->CreateRenderObject(obj);
    }

    // Spot Light Descriptions
    m_spotLights[0] = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::SpotLight;
        obj.properties.emplace("colour", "1.0 1.0 1.0");
        obj.properties.emplace("ambientIntensity", "0.1");
        obj.properties.emplace("diffuseIntensity", "0.6");
        obj.properties.emplace("position", "-400.0 650.0 -450.0");
        obj.properties.emplace("attenuation.constant", "1.0");
        obj.properties.emplace("attenuation.linear", "0.0");
        obj.properties.emplace("attenuation.exp", "0.0");
        obj.properties.emplace("coneDirection", "0.0 -1.0 -1.0");
        obj.properties.emplace("cosineConeAngle", "0.86"); // ~cos(30 degrees)
        m_spotLights[0] = m_renderer->CreateRenderObject(obj);
    }

    m_spotLights[1] = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::SpotLight;
        obj.properties.emplace("colour", "1.0 1.0 1.0");
        obj.properties.emplace("ambientIntensity", "0.1");
        obj.properties.emplace("diffuseIntensity", "0.6");
        obj.properties.emplace("position", "0.0 650.0 -450.0");
        obj.properties.emplace("attenuation.constant", "1.0");
        obj.properties.emplace("attenuation.linear", "0.0");
        obj.properties.emplace("attenuation.exp", "0.0");
        obj.properties.emplace("coneDirection", "0.0 -1.0 -1.0");
        obj.properties.emplace("cosineConeAngle", "0.86"); // ~cos(30 degrees)
        m_spotLights[1] = m_renderer->CreateRenderObject(obj);
    }

    m_spotLights[2] = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::SpotLight;
        obj.properties.emplace("colour", "1.0 1.0 1.0");
        obj.properties.emplace("ambientIntensity", "0.1");
        obj.properties.emplace("diffuseIntensity", "0.6");
        obj.properties.emplace("position", "400.0 650.0 -450.0");
        obj.properties.emplace("attenuation.constant", "1.0");
        obj.properties.emplace("attenuation.linear", "0.0");
        obj.properties.emplace("attenuation.exp", "0.0");
        obj.properties.emplace("coneDirection", "0.0 -1.0 -1.0");
        obj.properties.emplace("cosineConeAngle", "0.86"); // ~cos(30 degrees)
        m_spotLights[2] = m_renderer->CreateRenderObject(obj);
    }

    m_spotLights[3] = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::SpotLight;
        obj.properties.emplace("colour", "1.0 1.0 1.0");
        obj.properties.emplace("ambientIntensity", "0.1");
        obj.properties.emplace("diffuseIntensity", "0.6");
        obj.properties.emplace("position", "1293.0 886.0 -1634.0");
        obj.properties.emplace("attenuation.constant", "2.0");
        obj.properties.emplace("attenuation.linear", "0.0");
        obj.properties.emplace("attenuation.exp", "0.0");
        obj.properties.emplace("coneDirection", "-0.37 -0.49 0.78");
        obj.properties.emplace("cosineConeAngle", "0.86"); // ~cos(30 degrees)
        m_spotLights[3] = m_renderer->CreateRenderObject(obj);
    }

    m_spotLights[4] = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::SpotLight;
        obj.properties.emplace("colour", "1.0 1.0 1.0");
        obj.properties.emplace("ambientIntensity", "0.1");
        obj.properties.emplace("diffuseIntensity", "0.6");
        obj.properties.emplace("position", "400.0 590.0 550.0");
        obj.properties.emplace("attenuation.constant", "2.0");
        obj.properties.emplace("attenuation.linear", "0.0");
        obj.properties.emplace("attenuation.exp", "0.0");
        obj.properties.emplace("coneDirection", "-1.5 -1.0 0.0");
        obj.properties.emplace("cosineConeAngle", "0.86"); // ~cos(30 degrees)
        m_spotLights[4] = m_renderer->CreateRenderObject(obj);
    }

    m_spotLights[5] = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::SpotLight;
        obj.properties.emplace("colour", "1.0 1.0 1.0");
        obj.properties.emplace("ambientIntensity", "0.1");
        obj.properties.emplace("diffuseIntensity", "0.6");
        obj.properties.emplace("position", "-400.0 590.0 550.0");
        obj.properties.emplace("attenuation.constant", "2.0");
        obj.properties.emplace("attenuation.linear", "0.0");
        obj.properties.emplace("attenuation.exp", "0.0");
        obj.properties.emplace("coneDirection", "1.5 -1.0 0.0");
        obj.properties.emplace("cosineConeAngle", "0.86"); // ~cos(30 degrees)
        m_spotLights[5] = m_renderer->CreateRenderObject(obj);
    }

    m_spotLights[6] = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::SpotLight;
        obj.properties.emplace("colour", "1.0 1.0 1.0");
        obj.properties.emplace("ambientIntensity", "0.1");
        obj.properties.emplace("diffuseIntensity", "0.6");
        obj.properties.emplace("position", "-1427.0 918.0 -1740.0");
        obj.properties.emplace("attenuation.constant", "2.0");
        obj.properties.emplace("attenuation.linear", "0.0");
        obj.properties.emplace("attenuation.exp", "0.0");
        obj.properties.emplace("coneDirection", "0.84 -0.51 0.15");
        obj.properties.emplace("cosineConeAngle", "0.86"); // ~cos(30 degrees)
        m_spotLights[6] = m_renderer->CreateRenderObject(obj);
    }
}

void Game::LoadEnts()
{
    // Camera
    m_camera = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(0.0f, 20.0f, -150.0f);
        physical.orientation = glm::quat();
        m_entity_system.AttachComponent(m_camera, &physical);
    }


    // Room
    m_roomEnt = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        m_entity_system.AttachComponent(m_roomEnt, &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_roomMesh;
        graphical.material = m_wallBumpMaterial;
        m_entity_system.AttachComponent(m_roomEnt, &graphical);
    }

    // Floor
    m_floorEnt = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(0.0f, 0.0f, 0.0f);
        m_entity_system.AttachComponent(m_floorEnt, &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_floorMesh;
        graphical.material = m_woodFloorMaterial;
        m_entity_system.AttachComponent(m_floorEnt, &graphical);
    }

    // Pedestals and teapots
    m_pedestalTeapot1 = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(-400.0f, 185.0f, -600.0f);
        m_entity_system.AttachComponent(m_pedestalTeapot1, &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_pedestalMesh;
        graphical.material = m_pedestalBumpMaterial;
        m_entity_system.AttachComponent(m_pedestalTeapot1, &graphical);
    }

    m_teapotBumpedEnt = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(-400.0f, 370.0f, -600.0f);
        m_entity_system.AttachComponent(m_teapotBumpedEnt, &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_teapotMesh;
        graphical.material = m_teapotBumpMaterial;
        m_entity_system.AttachComponent(m_teapotBumpedEnt, &graphical);
    }

    m_pedestalTeapot2 = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(0.0f, 185.0f, -600.0f);
        m_entity_system.AttachComponent(m_pedestalTeapot2, &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_pedestalMesh;
        graphical.material = m_pedestalBumpMaterial;
        m_entity_system.AttachComponent(m_pedestalTeapot2, &graphical);
    }

    m_teapotMarbleEnt = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(0.0f, 370.0f, -600.0f);
        m_entity_system.AttachComponent(m_teapotMarbleEnt, &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_teapotMesh;
        graphical.material = m_teapotMarbleMaterial;
        m_entity_system.AttachComponent(m_teapotMarbleEnt, &graphical);
    }

    m_pedestalTeapot3 = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(400.0f, 185.0f, -600.0f);
        m_entity_system.AttachComponent(m_pedestalTeapot3, &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_pedestalMesh;
        graphical.material = m_pedestalBumpMaterial;
        m_entity_system.AttachComponent(m_pedestalTeapot3, &graphical);
    }

    m_teapotCelEnt = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(400.0f, 370.0f, -600.0f);
        m_entity_system.AttachComponent(m_teapotCelEnt, &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_teapotMesh;
        graphical.material = m_teapotCelMaterial;
        m_entity_system.AttachComponent(m_teapotCelEnt, &graphical);
    }

    m_moonLightEnt = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        m_entity_system.AttachComponent(m_moonLightEnt, &physical);

        GraphicalComponent graphical;
        graphical.directionalLight = m_moonLightDirectional;
        m_entity_system.AttachComponent(m_moonLightEnt, &graphical);
    }

    // Billboards
    m_billboardTextEnts[0] = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(-400.0f, 490.0f, -650.0f);
        m_entity_system.AttachComponent(m_billboardTextEnts[0], &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_billboardMesh;
        graphical.material = m_billboardTextMaterials[0];
        m_entity_system.AttachComponent(m_billboardTextEnts[0], &graphical);
    }

    m_billboardTextEnts[1] = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(0.0f, 490.0f, -650.0f);
        m_entity_system.AttachComponent(m_billboardTextEnts[1], &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_billboardMesh;
        graphical.material = m_billboardTextMaterials[1];
        m_entity_system.AttachComponent(m_billboardTextEnts[1], &graphical);
    }

    m_billboardTextEnts[2] = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(400.0f, 490.0f, -650.0f);
        m_entity_system.AttachComponent(m_billboardTextEnts[2], &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_billboardMesh;
        graphical.material = m_billboardTextMaterials[2];
        m_entity_system.AttachComponent(m_billboardTextEnts[2], &graphical);
    }

    // Paintings

    // gladimir 1998
    m_paintingEnts[0] = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(-450.0f, 400.0f, -1065.0f);
        physical.orientation = glm::quat(glm::vec3{0.0f, glm::pi<float>(), 0.0f});
        m_entity_system.AttachComponent(m_paintingEnts[0], &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_painting173;
        graphical.material = m_paintingMaterials[0];
        m_entity_system.AttachComponent(m_paintingEnts[0], &graphical);
    }

    // 
    m_paintingEnts[1] = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(-40.0f, 425.0f, 550.0f);
        physical.orientation = glm::quat(glm::vec3{ 0.0f, -glm::half_pi<float>(), 0.0f });
        m_entity_system.AttachComponent(m_paintingEnts[1], &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_painting100;
        graphical.material = m_paintingMaterials[1];
        m_entity_system.AttachComponent(m_paintingEnts[1], &graphical);
    }

    // bob
    m_paintingEnts[2] = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(900.0f, 400.0f, -685.0f);
        physical.orientation = glm::quat(glm::vec3{ 0.0f, glm::half_pi<float>(), 0.0f });
        m_entity_system.AttachComponent(m_paintingEnts[2], &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_painting100;
        graphical.material = m_paintingMaterials[2];
        m_entity_system.AttachComponent(m_paintingEnts[2], &graphical);
    }

    // bert stare
    m_paintingEnts[3] = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(70.0f, 425.0f, 550.0f);
        physical.orientation = glm::quat(glm::vec3{ 0.0f, glm::half_pi<float>(), 0.0f });
        m_entity_system.AttachComponent(m_paintingEnts[3], &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_paintint133;
        graphical.material = m_paintingMaterials[3];
        m_entity_system.AttachComponent(m_paintingEnts[3], &graphical);
    }

    // Spotlights
    m_spotLightEnts[0] = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        m_entity_system.AttachComponent(m_spotLightEnts[0], &physical);

        GraphicalComponent graphical;
        graphical.spotLight = m_spotLights[0];
        m_entity_system.AttachComponent(m_spotLightEnts[0], &graphical);
    }

    m_spotLightEnts[1] = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        m_entity_system.AttachComponent(m_spotLightEnts[1], &physical);

        GraphicalComponent graphical;
        graphical.spotLight = m_spotLights[1];
        m_entity_system.AttachComponent(m_spotLightEnts[1], &graphical);
    }

    m_spotLightEnts[2] = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        m_entity_system.AttachComponent(m_spotLightEnts[2], &physical);

        GraphicalComponent graphical;
        graphical.spotLight = m_spotLights[2];
        m_entity_system.AttachComponent(m_spotLightEnts[2], &graphical);
    }

    m_spotLightEnts[3] = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        m_entity_system.AttachComponent(m_spotLightEnts[3], &physical);

        GraphicalComponent graphical;
        graphical.spotLight = m_spotLights[3];
        m_entity_system.AttachComponent(m_spotLightEnts[3], &graphical);
    }

    /*m_spotLightEnts[4] = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        m_entity_system.AttachComponent(m_spotLightEnts[4], &physical);

        GraphicalComponent graphical;
        graphical.spotLight = m_spotLights[4];
        m_entity_system.AttachComponent(m_spotLightEnts[4], &graphical);
    }*/

    m_spotLightEnts[5] = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        m_entity_system.AttachComponent(m_spotLightEnts[5], &physical);

        GraphicalComponent graphical;
        graphical.spotLight = m_spotLights[5];
        m_entity_system.AttachComponent(m_spotLightEnts[5], &graphical);
    }

    m_spotLightEnts[6] = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        m_entity_system.AttachComponent(m_spotLightEnts[6], &physical);

        GraphicalComponent graphical;
        graphical.spotLight = m_spotLights[6];
        m_entity_system.AttachComponent(m_spotLightEnts[6], &graphical);
    }
}
