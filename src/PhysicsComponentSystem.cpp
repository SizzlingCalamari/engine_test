
#include "PhysicsComponentSystem.h"
#include "PhysicsWorld.h"
#include "PhysicsComponent.h"

PhysicsComponentSystem::PhysicsComponentSystem()
{
	m_PhysicsWorld = new PhysicsWorld();
}

PhysicsComponentSystem::~PhysicsComponentSystem()
{
	delete m_PhysicsWorld;
}

PhysicsComponent PhysicsComponentSystem::GetComponent(uint32_t ent)
{
	return PhysicsComponent(this, ent);
}

bool PhysicsComponentSystem::Initialize()
{
	m_PhysicsWorld->GetDynamicsWorld()->setGravity({ 0.0f, -9.8f, 0.0f });
	return true;
}

void PhysicsComponentSystem::Shutdown()
{
	for (auto &c : m_Components)
	{
		DeleteComponent(c.second);
	}
}

void PhysicsComponentSystem::AttachComponent(uint32_t ent)
{
	m_Components[ent] = nullptr;
}

void PhysicsComponentSystem::Update(uint32_t dt)
{
	m_PhysicsWorld->GetDynamicsWorld()->stepSimulation(dt / 1000000.0f);
}

void PhysicsComponentSystem::SetComponentInfo(uint32_t ent, const PhysicsComponentInfo &info)
{
	DeleteComponent(m_Components.at(ent));

	btDefaultMotionState *motionState = new btDefaultMotionState(info.transform);
	btRigidBody *body = new btRigidBody(info.mass, motionState, info.shape, info.inertia);

	m_Components[ent] = body;
	m_PhysicsWorld->GetDynamicsWorld()->addRigidBody(body);
}

void PhysicsComponentSystem::DeleteComponent(PhysicsComponentInternal *comp)
{
	if (comp)
	{
		auto *body = btRigidBody::upcast(comp);
		if (body)
		{
			delete body->getMotionState();
		}
		m_PhysicsWorld->GetDynamicsWorld()->removeCollisionObject(comp);
		delete comp;
	}
}
