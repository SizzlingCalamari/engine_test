

#include "CameraController_ThirdPerson.h"

#include "../EntitySystem/ComponentTable.h"
#include "../EntitySystem/PhysicalComponent.h"

#include "mathutils.h"

static inline bool EpsilonNotEqual(const glm::quat& a, const glm::quat& b)
{
    return glm::abs(glm::dot(a, b)) < 0.99999f;
}

void CameraController_ThirdPerson::Update(uint32 dt)
{
    assert(m_physical_components);
    const auto cameraEnt = m_camera_ent;
    auto physical_camera = m_physical_components->GetComponent(cameraEnt);
    const auto *physical_target = m_physical_components->PeekComponent(m_target_ent);

    const auto targetOrientation = physical_target->orientation;
    if (EpsilonNotEqual(physical_camera.orientation, targetOrientation))
    {
        // linear interpolation between positions
        physical_camera.orientation = glm::slerp(physical_camera.orientation, targetOrientation, 0.05f);
    }

    const auto forward = math::forward(physical_camera.orientation);
    physical_camera.position = physical_target->position - (forward * m_radius_from_target);

    // Set the updated component.
    m_physical_components->EditComponent(cameraEnt, &physical_camera);
}
