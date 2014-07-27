
#include "BillboardController.h"

#include "../EntitySystem/ComponentTable.h"
#include "../EntitySystem/PhysicalComponent.h"

#include "mathutils.h"

void BillboardController::Update()
{
    assert(m_physical_components);

    auto *camera = m_physical_components->PeekComponent(m_cameraEnt);
    auto cameraForward = math::forward(camera->orientation);

    int numBillboards = static_cast<int>(m_billboardEnts.size());
    for (int i = 0; i < numBillboards; ++i)
    {
        uint ent = m_billboardEnts[i];
        auto initialForward = m_billboardData[i].initialDirection;
        if (IsBillboardActive(cameraForward, initialForward))
        {
            auto billboard = m_physical_components->GetComponent(ent);
            auto *billboardData = m_billboardData.data() + i;
            // reorient the billboard
            OrientBillboard(camera, billboardData, &billboard);

            // apply changes
            m_physical_components->EditComponent(ent, &billboard);
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
