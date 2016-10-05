
#include "Thwomp.h"

#include "../EntitySystem/ComponentTable.h"
#include "../EntitySystem/PhysicalComponent.h"

#include <algorithm>

void Thwomp::Update(uint32 dt)
{
    assert(m_physical_components);
    assert(m_ent != 0);

    switch (m_state.state)
    {
        case ThwompState::START:
        {
            m_state.timer = static_cast<int>(m_data.groundHoldDuration * 1000.0f);
            m_state.state = ThwompState::GROUND_HOLD;
            break;
        }
        case ThwompState::GROUND_HOLD:
        {
            if(UpdateTimer(dt))
            {
                m_state.state = ThwompState::RISING;
            }
            break;
        }
        case ThwompState::RISING:
        {
            const float seconds = dt * 0.001f;
            const float positionOffset = seconds * m_data.riseSpeed;
            const float height = m_state.height + positionOffset;
            const float riseHeight = m_data.riseHeight;
            m_state.height = std::min(riseHeight, height);

            UpdatePosition(positionOffset);

            if(height >= riseHeight)
            {
                m_state.state = ThwompState::PEAK_HOLD;
                m_state.timer = static_cast<int>(m_data.peakHoldDuration * 1000.0f);
            }
            break;
        }
        case ThwompState::PEAK_HOLD:
        {
            if(UpdateTimer(dt))
            {
                m_state.state = ThwompState::FALLING;
            }
            break;
        }
        case ThwompState::FALLING:
        {
            const float seconds = dt * 0.001f;
            const float positionOffset = seconds * -m_data.fallSpeed;
            const float height = m_state.height + positionOffset;
            m_state.height = std::max(0.0f, height);

            UpdatePosition(positionOffset);

            if(height <= 0.0f)
            {
                if(m_onHitGroundFn)
                {
                    m_onHitGroundFn();
                }
                m_state.state = ThwompState::START;
            }
            break;
        }
    }
}

void Thwomp::UpdatePosition(float positionOffset)
{
    PhysicalComponent thwomp = m_physical_components->GetComponent(m_ent);
    thwomp.position.y += positionOffset;
    m_physical_components->EditComponent(m_ent, &thwomp);
}

bool Thwomp::UpdateTimer(uint dt)
{
    const int timer = m_state.timer - dt;
    m_state.timer = std::max(0, timer);
    return(timer <= 0);
}
