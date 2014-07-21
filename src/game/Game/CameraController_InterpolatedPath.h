
#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

template<typename T> class ComponentTable;
struct PhysicalComponent;

class CameraController_InterpolatedPath
{
public:
    CameraController_InterpolatedPath():
        m_physical_components(nullptr),
        m_cameraEnt(0),
        m_startTime(0),
        m_curTime(0),
        m_period(5000),
        m_shouldLoop(false)
    {
    }

    void SetComponentTables(ComponentTable<PhysicalComponent>* physical)
    {
        m_physical_components = physical;
    }

    void SetCameraEnt(uint ent)
    {
        m_cameraEnt = ent;
    }

    void SetShouldLoop(bool loop)
    {
        m_shouldLoop = true;
    }

    void SetStartTime(uint time)
    {
        m_startTime = time;
        m_curTime = time;
    }

    void SetPeriod(uint period)
    {
        m_period = period;
    }

    void AddControlPoint(const glm::vec3& position,
                         const glm::quat& orientation)
    {
        m_positions.emplace_back(position);
        m_orientations.emplace_back(orientation);
    }

    void Update(uint32 dt);

private:
    ComponentTable<PhysicalComponent> *m_physical_components;
    uint m_cameraEnt;
    uint m_startTime;
    uint m_curTime;
    uint m_period;
    bool m_shouldLoop;

    std::vector<glm::vec3> m_positions;
    std::vector<glm::quat> m_orientations;
};
