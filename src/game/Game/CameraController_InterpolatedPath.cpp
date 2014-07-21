

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

    uint timePerControlPoint = m_period / numPoints;
    float interpValue = (float)(m_curTime % timePerControlPoint) / timePerControlPoint;

    uint point2 = (m_curTime / timePerControlPoint) % numPoints;
    uint point1 = (point2 - 1) % numPoints;
    uint point3 = (point2 + 1) % numPoints;
    uint point4 = (point2 + 2) % numPoints;

    auto camera = m_physical_components->GetComponent(m_cameraEnt);
    camera.position = glm::catmullRom(m_positions[point1], m_positions[point2],
                                      m_positions[point3], m_positions[point4],
                                      interpValue);
    
    // Set the updated component.
    m_physical_components->EditComponent(m_cameraEnt, &camera);

    // update the current time
    m_curTime = (m_curTime + dt) % m_period;
}
