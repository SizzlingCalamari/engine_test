
#pragma once

#include "Billboard.h"
#include <glm/vec3.hpp>
#include <vector>

template<typename T> class ComponentTable;
struct PhysicalComponent;

class BillboardController
{
public:
    BillboardController():
        m_physical_components(nullptr),
        m_cameraEnt(0)
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

    void AddBillboard(uint ent, const CylindricalBillboard& data)
    {
        m_billboardEnts.emplace_back(ent);
        m_billboardData.emplace_back(data);
    }

    void Update();

private:
    // Active if cameraForward dot billboardForward is negative.
    // Kind of a hack since this should check the camera fov
    // for the billboard too, but oh well.
    bool IsBillboardActive(const glm::vec3& cameraForward,
                           const glm::vec3& billboardInitialForward);

    // Reorients with respect to the angle constraints
    // on the billboard.
    void OrientBillboard(const PhysicalComponent* camera,
                         const CylindricalBillboard* constraints,
                         PhysicalComponent* billboard);

private:
    ComponentTable<PhysicalComponent> *m_physical_components;
    std::vector<uint> m_billboardEnts;
    std::vector<CylindricalBillboard> m_billboardData;
    uint m_cameraEnt;
};
