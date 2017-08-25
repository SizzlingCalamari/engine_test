
#pragma once

template<typename T> class ComponentTable;
struct PhysicalComponent;

struct KartState
{
    float throttlePercent;
    float torquePercent;

    // meters
    float wheelRadius = 0.127f;

    // Newton meters
    float stallTorque = 0.325f;

    // RPM
    float noLoadSpeed = 1200.0f;
};


// http://www.asawicki.info/Mirror/Car%20Physics%20for%20Games/Car%20Physics%20for%20Games.html

// http://engines.honda.com/models/model-detail/gxh50
// http://www.northerntool.com/images/downloads/charts/60025.pdf
// http://lancet.mit.edu/motors/motors3.html
// circumference = 2*pi*r = 2*pi*0.127 = 0.79796386 m
// 1500.0f rpm * 1min/60sec * 0.79796386m/1rot = ~20 m/s (~45mph)

// y(x) = Ax^2 + Bx + C

// y(3000) = 2.3
// 2.3 = A*9000000 + B*3000 + C
// 2.3 - A*9000000 - B*3000 = C

// y(4500) = 2.7
// 2.7 = A*20250000 + B*4500 + C

// y(8000) = 1.3
// 1.3 = A*64000000 + B*8000 + C


class Kart
{
public:
    Kart():
        m_physical_components(nullptr),
        m_ent(0)
    {
    }

    void SetComponentTables(ComponentTable<PhysicalComponent>* physical)
    {
        m_physical_components = physical;
    }

    void SetKartEnt(uint ent)
    {
        m_ent = ent;
    }

    void Update(uint32 dt);

private:
    ComponentTable<PhysicalComponent> *m_physical_components;
    uint m_ent;
};
