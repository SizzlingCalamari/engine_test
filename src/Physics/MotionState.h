
#pragma once

#include "btBulletDynamicsCommon.h"
#include <vector>
#include <glm/glm.hpp>

class MotionState: public btMotionState
{
public:
    MotionState(std::vector<MotionState*>* changelist,
                const glm::vec3& position,
                uint ent):
                m_changelist(changelist),
                m_position(position),
                m_ent(ent)
    {
        assert(changelist);
    }

    virtual void getWorldTransform(btTransform& worldTrans) const
    {
        btVector3 position(m_position.x, m_position.y, m_position.z);
        worldTrans.setOrigin(position);
    }

    // Bullet only calls the update of worldtransform for active objects
    virtual void setWorldTransform(const btTransform& worldTrans)
    {
        auto origin = worldTrans.getOrigin();
        m_position.x = origin.x();
        m_position.y = origin.y();
        m_position.z = origin.z();
        m_changelist->emplace_back(this);
    }

    const glm::vec3& GetPosition() const
    {
        return m_position;
    }

    uint GetEnt() const
    {
        return m_ent;
    }

private:
    std::vector<MotionState*> *m_changelist;
    glm::vec3 m_position;
    uint m_ent;
};
