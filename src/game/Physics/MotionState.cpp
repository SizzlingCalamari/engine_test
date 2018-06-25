
#include "MotionState.h"

MotionState::MotionState(std::vector<MotionState*>* changelist,
            const glm::vec3& position,
            const glm::quat& orientation,
            uint ent):
            m_changelist(changelist),
            m_position(position),
            m_orientation(orientation),
            m_ent(ent)
{
    assert(changelist);
}

MotionState::~MotionState() = default;

void MotionState::getWorldTransform(btTransform& worldTrans) const
{
    btVector3 position(m_position.x, m_position.y, m_position.z);
    worldTrans.setOrigin(position);

    btQuaternion orientation(m_orientation.x, m_orientation.y,
                                m_orientation.z, m_orientation.w);
    worldTrans.setRotation(orientation);
}

void MotionState::setWorldTransform(const btTransform& worldTrans)
{
    auto &origin = worldTrans.getOrigin();
    m_position.x = origin.x();
    m_position.y = origin.y();
    m_position.z = origin.z();

    auto orientation = worldTrans.getRotation();
    m_orientation.x = orientation.x();
    m_orientation.y = orientation.y();
    m_orientation.z = orientation.z();
    m_orientation.w = orientation.w();

    m_changelist->emplace_back(this);
}
