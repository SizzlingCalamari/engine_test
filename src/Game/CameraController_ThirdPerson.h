
#pragma once

template<typename T> class ComponentTable;
struct PhysicalComponent;

class CameraController_ThirdPerson
{
public:
    void SetComponentTables(ComponentTable<PhysicalComponent>* physical)
    {
        m_physical_components = physical;
    }

    void SetCameraEnt(uint ent)
    {
        m_camera_ent = ent;
    }

    void SetTargetEnt(uint ent)
    {
        m_target_ent = ent;
    }

    void SetRadiusFromTarget(float radius)
    {
        m_radius_from_target = radius;
    }

    void Update(uint32 dt);

private:
    ComponentTable<PhysicalComponent> *m_physical_components;
    uint m_camera_ent;
    uint m_target_ent;
    float m_radius_from_target;
};
