
#pragma once

#include <queue>
#include <vector>

class IEntityComponentSystem;

class EntitySystem
{
public:
    EntitySystem();
    ~EntitySystem();

    bool Initialize();
    void Shutdown();

    uint32 CreateEntity(const std::vector<uint8> &components);
    void DestroyEntity(uint32 ent);

	uint32 RegisterComponentSystem(IEntityComponentSystem *pSystem, uint32 depends_on = 0);

	void Update(uint32 dt);

private:
    const static uint32 MAX_ENTS = 2048;

private:
    std::deque<uint32> m_FreeEnts;
    std::vector<uint32> m_DestroyedEnts;

    std::vector<IEntityComponentSystem*> m_Systems;
};
