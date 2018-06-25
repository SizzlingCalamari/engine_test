
#include "BillboardController.h"

#include "../EntitySystem/ComponentTable.h"
#include "../EntitySystem/PhysicalComponent.h"

#include "mathutils.h"

void BillboardController::Update()
{
    assert(m_physical_components);

    auto *camera = m_physical_components->PeekComponent(m_cameraEnt);
    auto cameraForward = math::forward(camera->orientation);

    for(const BillboardEnt& billboardEnt : m_billboards)
    {
        const CylindricalBillboard& billboard = billboardEnt.billboard;
        const uint ent = billboardEnt.ent;
        auto initialForward = billboard.initialDirection;
        if (IsBillboardActive(cameraForward, initialForward))
        {
            PhysicalComponent billboardPhysical = m_physical_components->GetComponent(ent);
            OrientBillboard(camera, &billboard, &billboardPhysical);

            m_physical_components->EditComponent(ent, &billboardPhysical);
        }
    }
}

bool BillboardController::IsBillboardActive(const glm::vec3& cameraForward,
                                            const glm::vec3& billboardInitialForward)
{
    return true;//(glm::dot(cameraForward, billboardInitialForward) < 0);
}

void BillboardController::OrientBillboard(const PhysicalComponent* camera,
                                          const CylindricalBillboard* constraints,
                                          PhysicalComponent* billboard)
{
    auto forward = glm::normalize(camera->position - billboard->position);
    if (constraints->maxAngleFromForward > 0.0f)
    {
        forward = math::limit_angle(constraints->initialDirection,
                                    forward,
                                    constraints->maxAngleFromForward);
        forward = glm::normalize(forward);
    }

    // TODO: guard against invalid cross product
    auto right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), forward));
    auto up = glm::normalize(glm::cross(forward, right));

    auto newBasis = glm::mat3(right, up, forward);
    billboard->orientation = glm::quat(newBasis);
}
