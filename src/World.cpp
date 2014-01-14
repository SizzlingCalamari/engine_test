
#include "World.h"
#include "EntitySystem.h"
#include "PhysicsComponentSystem.h"
#include "PhysicsComponent.h"

World::World()
{
	m_EntSys = new EntitySystem();

	auto *phys = new PhysicsComponentSystem();
	m_Physics.first = phys;
	m_Physics.second = m_EntSys->RegisterComponentSystem(phys);

	/*auto *graphics = new GraphicsComponentSystem();
	m_Graphics.first = graphics;
	m_Grapihcs.second = m_EntSys->RegisterComponentSystem(graphics);*/
}

World::~World()
{
	delete m_Physics.first;
	delete m_EntSys;
}

bool World::Initialize()
{
	bool ret = m_EntSys->Initialize();

	PhysicsComponentInfo info;
	info.shape = new btBoxShape(btVector3(btScalar(110.), btScalar(1.), btScalar(110.)));

	uint32_t ent = m_EntSys->CreateEntity({ m_Physics.second/*, m_Graphics.second*/ });
	auto pc = m_Physics.first->GetComponent(ent);
	pc.SetInfo(info);

	//auto gc = m_Graphics.first->GetComponent(ent);

	return ret;
}

void World::Shutdown()
{
	m_EntSys->Shutdown();
}

#include <chrono>

uint64_t CurTimeMicro()
{
	using namespace std::chrono;
	auto micro = time_point_cast<microseconds>(high_resolution_clock::now());
	return micro.time_since_epoch().count();
}

//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>

void World::Update(uint32_t dt)
{
	static uint64_t acc = 0;
	static uint64_t cur = CurTimeMicro();
	static uint64_t last = cur - dt;

	uint64_t time_now = CurTimeMicro();
	uint64_t frame_time = time_now - last;
	if (frame_time > 100000)
	{
		frame_time = 100000;
	}
	last = time_now;
	acc += frame_time;

	if (acc >= dt)
	{
		do
		{
			m_EntSys->Update(dt);
			cur += dt;
			acc -= dt;
		} while (acc >= dt);
	}
	else if ((dt-acc) > 1000)
	{
		//Sleep((dt-acc)/2000);
	}
}
