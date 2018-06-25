
#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "LinearMath/btMotionState.h"

class MotionState: public btMotionState
{
public:
    MotionState(std::vector<MotionState*>* changelist,
                const glm::vec3& position,
                const glm::quat& orientation,
                uint ent);
    ~MotionState();

    virtual void getWorldTransform(btTransform& worldTrans) const;

    // Bullet only calls the update of worldtransform for active objects
    virtual void setWorldTransform(const btTransform& worldTrans);

    const glm::vec3& GetPosition() const
    {
        return m_position;
    }

    const glm::quat& GetOrientation() const
    {
        return m_orientation;
    }

    uint GetEnt() const
    {
        return m_ent;
    }

private:
    std::vector<MotionState*> *m_changelist;
    glm::vec3 m_position;
    glm::quat m_orientation;
    uint m_ent;
};
