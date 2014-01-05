
#pragma once

#include <stdint.h>
#include <tuple>

class EntitySystem;
class PhysicsComponentSystem;

class World
{
public:
    World();
    ~World();

    bool Initialize();
    void Shutdown();

    void Update(uint32_t dt);

private:
    EntitySystem *m_EntSys;

	std::pair<PhysicsComponentSystem*, uint8_t> m_Physics;
};
