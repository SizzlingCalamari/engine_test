
#pragma once

#include <functional>

template<typename T> class ComponentTable;
struct PhysicalComponent;

struct ThwompData
{
    float riseHeight = 150.0f;
    float riseSpeed = 75.0f;
    float fallSpeed = 600.0f;
    float peakHoldDuration = 3.0f;
    float groundHoldDuration = 1.5f;
};

struct ThwompState
{
    enum State
    {
        START = 0,
        GROUND_HOLD,
        RISING,
        PEAK_HOLD,
        FALLING
    };

    State state = START;
    int timer = 0;
    float height = 0.0f;
};

class Thwomp
{
public:
    Thwomp():
        m_physical_components(nullptr),
        m_ent(0),
        m_data()
    {
    }

    void SetComponentTables(ComponentTable<PhysicalComponent>* physical)
    {
        m_physical_components = physical;
    }

    void SetThwompEnt(uint ent)
    {
        m_ent = ent;
    }

    void OnHitGround(const std::function<void()>& fn)
    {
        m_onHitGroundFn = fn;
    }

    void SetData(const ThwompData& data)
    {
        assert(data.riseSpeed > 0.0f);
        assert(data.fallSpeed > 0.0f);
        assert(data.peakHoldDuration >= 0.0f);
        assert(data.groundHoldDuration >= 0.0f);
        m_data = data;
    }

    void Update(uint32 dt);

private:
    void UpdatePosition(float positionOffset);
    bool UpdateTimer(uint dt);

private:
    ComponentTable<PhysicalComponent> *m_physical_components;
    uint m_ent;
    ThwompData m_data;
    ThwompState m_state;
    std::function<void()> m_onHitGroundFn;
};
