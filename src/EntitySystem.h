
#pragma once

#include <stdint.h>
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

    uint32_t CreateEntity(const std::vector<uint8_t> &components);
    void DestroyEntity(uint32_t ent);

	uint32_t RegisterComponentSystem(IEntityComponentSystem *pSystem, uint32_t depends_on = 0);

	void Update(uint32_t dt);

private:
    const static uint32_t MAX_ENTS = 2048;

private:
    std::deque<uint32_t> m_FreeEnts;
    std::vector<uint32_t> m_DestroyedEnts;

    std::vector<IEntityComponentSystem*> m_Systems;
};
