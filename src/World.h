
#pragma once

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

    void Update(uint32 dt);

private:
    EntitySystem *m_EntSys;

	std::pair<PhysicsComponentSystem*, uint8> m_Physics;
};
