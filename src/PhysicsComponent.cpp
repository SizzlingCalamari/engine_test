
#include "PhysicsComponent.h"
#include "PhysicsComponentSystem.h"

void PhysicsComponent::SetInfo(const PhysicsComponentInfo &info)
{
	m_PhysicsSystem->SetComponentInfo(m_ent, info);
}

PhysicsComponent::PhysicsComponent(PhysicsComponentSystem *sys, uint32_t ent):
	m_PhysicsSystem(sys),
	m_ent(ent)
{
}
