

#include "CameraController_ThirdPerson.h"

#include "../EntitySystem/ComponentTable.h"
#include "../EntitySystem/PhysicalComponent.h"

#include "mathutils.h"

void CameraController_ThirdPerson::Update(uint32 dt)
{
    assert(m_physical_components);
    assert(m_physical_components->HasComponent(m_camera_ent));
    assert(m_physical_components->HasComponent(m_target_ent));
    auto physical_camera = m_physical_components->GetComponent(m_camera_ent);
    auto *physical_target = m_physical_components->PeekComponent(m_target_ent);

    // linear interpolation between positions
    auto new_orientation = glm::slerp(physical_camera.orientation, physical_target->orientation, 0.05f);
    auto forward = math::forward(new_orientation);
    physical_camera.position = physical_target->position - (forward * m_radius_from_target);
    physical_camera.orientation = new_orientation;
    
    // Set the updated component.
    m_physical_components->EditComponent(m_camera_ent, &physical_camera);
}
