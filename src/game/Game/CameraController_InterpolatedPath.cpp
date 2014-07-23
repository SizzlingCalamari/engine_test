

#include "CameraController_InterpolatedPath.h"

#include "../EntitySystem/ComponentTable.h"
#include "../EntitySystem/PhysicalComponent.h"

#include "mathutils.h"

#include <glm/gtx/spline.hpp>

void CameraController_InterpolatedPath::Update(uint32 dt)
{
    assert(m_physical_components);
    assert(m_positions.size() >= 4);

    auto numPoints = m_positions.size();

    uint timePerControlPoint = m_timePerControlPoint;
    float interpValue = (float)(m_curTime % timePerControlPoint) / timePerControlPoint;

    uint point1 = (m_curTime / timePerControlPoint) % numPoints;
    uint point2 = (point1 + 1) % numPoints;

    auto camera = m_physical_components->GetComponent(m_cameraEnt);
    camera.position = InterpPosition(point1, point2, interpValue);
    camera.orientation = InterpOrientation(point1, point2, interpValue);
    
    // Set the updated component.
    m_physical_components->EditComponent(m_cameraEnt, &camera);

    // update the current time
    m_curTime = (m_curTime + dt) % (timePerControlPoint*numPoints);
}

glm::vec3 CameraController_InterpolatedPath::InterpPosition(uint index1, uint index2, const float t)
{
    auto numPoints = m_positions.size();
    uint index0 = (index1 - 1) % numPoints;
    uint index3 = (index2 + 1) % numPoints;
    return glm::catmullRom(m_positions[index0], m_positions[index1],
                           m_positions[index2], m_positions[index3], t);
}

glm::quat CameraController_InterpolatedPath::InterpOrientation(uint index1, uint index2, const float t)
{
    return glm::slerp(m_orientations[index1], m_orientations[index2], t);
}
