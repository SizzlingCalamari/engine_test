
#include "EntitySystem.h"
#include "PhysicsComponentSystem.h"

EntitySystem::EntitySystem()
{
}

EntitySystem::~EntitySystem()
{
}

bool EntitySystem::Initialize()
{
    std::deque<uint32_t> ents;
    for (uint_fast32_t i = 0; i < MAX_ENTS; ++i)
    {
        ents.emplace_back(i);
    }
    m_FreeEnts = std::move(ents);
    m_DestroyedEnts.reserve(MAX_ENTS/16);

    for (auto sys : m_Systems)
    {
        sys->Initialize();
    }
	return true;
}

void EntitySystem::Shutdown()
{
    for (auto sys : m_Systems)
    {
        sys->Shutdown();
    }
}

uint32_t EntitySystem::CreateEntity(const std::vector<uint8_t> &components)
{
    uint32_t ent = m_FreeEnts.front();
    m_FreeEnts.pop_back();

	for (uint_fast32_t i : components)
	{
		m_Systems[i]->AttachComponent(ent);
	}
    return ent;
}

void EntitySystem::DestroyEntity(uint32_t ent)
{
    m_DestroyedEnts.emplace_back(ent);
}

uint32_t EntitySystem::RegisterComponentSystem(IEntityComponentSystem *pSystem, uint32_t depends_on /*= 0*/)
{
	m_Systems.emplace_back(pSystem);
	return m_Systems.size() - 1;
}

void EntitySystem::Update(uint32_t dt)
{
	if (!m_DestroyedEnts.empty())
	{
		m_FreeEnts.insert(m_FreeEnts.begin(),
			m_DestroyedEnts.begin(), m_DestroyedEnts.end());
		m_DestroyedEnts.clear();
	}

	for (auto sys : m_Systems)
	{
		sys->Update(dt);
	}
}
