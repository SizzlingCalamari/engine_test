
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
    m_teapotTextured(0),
    m_teapotBumped(0),
    m_pedestal(0),
    m_moonLightEnt(0),
    m_billboardTextMaterials(),
    m_billboardTextEnts(),
    m_inputIdZ(0),
    m_activeCameraType(Camera_InterpolatedPath)
{
}

void Game::Initialize(const EngineContext& engine)
{
    using namespace std::placeholders;
    KeyboardContext keyboardContext;
    {
        KeyCombination temp = { SDL_SCANCODE_Z };
        keyboardContext.AddMapping(temp, std::bind(&Game::InputEventCallback, this, _1));
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
    m_thirdperson_controller.SetTargetEnt(m_teapotTextured);
    m_thirdperson_controller.SetRadiusFromTarget(75.0f);
    
    m_cameraPathController.SetCameraEnt(m_camera);
    m_cameraPathController.SetPeriod(15000);
    m_cameraPathController.SetShouldLoop(false);
    m_cameraPathController.SetStartTime(0);

    float mid = 75.0f * glm::root_two<float>();
    m_cameraPathController.AddControlPoint(glm::vec3{ 0.0f, 20.0f, -150.0f }, glm::quat(glm::vec3{ 0.0f, 0.0f, 0.0f }));
    //m_cameraPathController.AddControlPoint(glm::vec3{ mid, 30.0f, -mid }, glm::quat(glm::vec3{ 0.0f, 0.0f, 0.0f }));
    m_cameraPathController.AddControlPoint(glm::vec3{ 150.0f, 40.0f, 0.0f }, glm::quat(glm::vec3{ 0.0f, -glm::half_pi<float>(), 0.0f }));
    //m_cameraPathController.AddControlPoint(glm::vec3{ mid, 50.0f, mid }, glm::quat(glm::vec3{ 0.0f, 0.0f, 0.0f }));
    m_cameraPathController.AddControlPoint(glm::vec3{ 0.0f, 40.0f, 150.0f }, glm::quat(glm::vec3{ 0.0f, glm::pi<float>(), 0.0f }));
    //m_cameraPathController.AddControlPoint(glm::vec3{ -mid, 30.0f, mid }, glm::quat(glm::vec3{ 0.0f, 0.0f, 0.0f }));
    m_cameraPathController.AddControlPoint(glm::vec3{ -150.0f, 20.0f, 0.0f }, glm::quat(glm::vec3{ 0.0f, glm::half_pi<float>(), 0.0f }));
    //m_cameraPathController.AddControlPoint(glm::vec3{ -mid, 30.0f, -mid }, glm::quat(glm::vec3{ 0.0f, 0.0f, 0.0f }));

    m_billboardController.SetCameraEnt(m_camera);
    {
        CylindricalBillboard billboard;
        billboard.initialDirection = glm::vec3(0.0f, 0.0f, 1.0f);
        // 30 degrees
        billboard.maxAngleFromForward = glm::half_pi<float>() / 3.0f;
        m_billboardController.AddBillboard(m_billboardTextEnts[0], billboard);
    }
}

void Game::Shutdown()
{
    m_entity_system.DestroyEntity(m_camera);
    m_entity_system.DestroyEntity(m_floorEnt);
    m_entity_system.DestroyEntity(m_teapotTextured);
    m_entity_system.DestroyEntity(m_teapotBumped);
    m_entity_system.DestroyEntity(m_pedestal);

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
        m_thirdperson_controller.Update(dt);
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
        float key_factor = float(dt * 0.5);
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
    m_floorMesh = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::MeshObject;
        obj.properties.emplace("meshFile", "models/floor.obj");
        m_floorMesh = m_renderer->CreateRenderObject(obj);
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
        obj.properties.emplace("diffuseMapFile", "models/teapot/default.png");
        obj.properties.emplace("specularIntensity", "1.0");
        obj.properties.emplace("specularPower", "32.0");
        obj.properties.emplace("noiseDiffuseMap", "1");
        m_teapotMarbleMaterial = m_renderer->CreateRenderObject(obj);
    }

    m_teapotBumpMaterial = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::Material;
        obj.properties.emplace("diffuseMapFile", "models/teapot/default.png");
        obj.properties.emplace("specularIntensity", "1.0");
        obj.properties.emplace("specularPower", "32.0");
        obj.properties.emplace("noiseBumpMap", "1");
        m_teapotBumpMaterial = m_renderer->CreateRenderObject(obj);
    }

    m_teapotCelMaterial = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::Material;
        obj.properties.emplace("diffuseMapFile", "models/teapot/default.png");
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
        obj.properties.emplace("ambientIntensity", "0.1");
        obj.properties.emplace("diffuseIntensity", "0.2");
        obj.properties.emplace("direction", "-1.0 -1.0 -1.0");
        m_moonLightDirectional = m_renderer->CreateRenderObject(obj);
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

    m_billboardMesh = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::MeshObject;
        obj.properties.emplace("meshFile", "models/billboard.obj");
        m_billboardMesh = m_renderer->CreateRenderObject(obj);
    }

    m_billboardTextMaterials[0] = 0;
    {
        RenderObject obj;
        obj.type = RenderObject::Material;
        obj.properties.emplace("diffuseMapFile", "textures/celshading.png");
        obj.properties.emplace("specularIntensity", "1.0");
        obj.properties.emplace("specularPower", "32.0");
        m_billboardTextMaterials[0] = m_renderer->CreateRenderObject(obj);
    }
}

void Game::LoadEnts()
{
    m_camera = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(0.0f, 20.0f, -150.0f);
        physical.orientation = glm::quat();
        m_entity_system.AttachComponent(m_camera, &physical);
    }

    m_floorEnt = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(0.0f, -100.0f, 0.0f);
        m_entity_system.AttachComponent(m_floorEnt, &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_floorMesh;
        graphical.material = m_jesusMaterial;
        m_entity_system.AttachComponent(m_floorEnt, &graphical);
    }

    m_teapotTextured = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        m_entity_system.AttachComponent(m_teapotTextured, &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_teapotMesh;
        graphical.material = m_teapotDefaultMaterial;
        m_entity_system.AttachComponent(m_teapotTextured, &graphical);
    }

    m_teapotBumped = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(100.0f, 0.0f, 0.0f);
        m_entity_system.AttachComponent(m_teapotBumped, &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_teapotMesh;
        graphical.material = m_teapotDefaultMaterial;
        m_entity_system.AttachComponent(m_teapotBumped, &graphical);
    }

    m_pedestal = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        m_entity_system.AttachComponent(m_pedestal, &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_pedestalMesh;
        graphical.material = m_pedestalBumpMaterial;
        m_entity_system.AttachComponent(m_pedestal, &graphical);
    }

    m_moonLightEnt = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        m_entity_system.AttachComponent(m_moonLightEnt, &physical);

        GraphicalComponent graphical;
        graphical.directionalLight = m_moonLightDirectional;
        m_entity_system.AttachComponent(m_moonLightEnt, &graphical);
    }

    m_billboardTextEnts[0] = m_entity_system.CreateEntity();
    {
        PhysicalComponent physical;
        physical.position = glm::vec3(200.0f, 0.0f, 0.0f);
        m_entity_system.AttachComponent(m_billboardTextEnts[0], &physical);

        GraphicalComponent graphical;
        graphical.mesh = m_billboardMesh;
        graphical.material = m_billboardTextMaterials[0];
        m_entity_system.AttachComponent(m_billboardTextEnts[0], &graphical);
    }
}
