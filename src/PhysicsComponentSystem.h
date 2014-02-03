
#pragma once

#include "IEntityComponentSystem.h"
#include "PhysicsComponent.h"
#include <unordered_map>

class PhysicsWorld;
struct PhysicsComponentInfo;

class PhysicsComponentSystem: public IEntityComponentSystem
{
	friend class PhysicsComponent;

public:
    PhysicsComponentSystem();
    ~PhysicsComponentSystem();

	PhysicsComponent GetComponent(uint32 ent);

public:
	// IEntityComponentSystem
	//
    virtual bool Initialize();

    virtual void Shutdown();

protected:
	virtual void AttachComponent(uint32 ent);

	virtual void Update(uint32 dt);
	//
	// =======

	void SetComponentInfo(uint32 ent, const PhysicsComponentInfo &info);

private:
	void DeleteComponent(PhysicsComponentInternal *comp);

private:
	PhysicsWorld *m_PhysicsWorld;
	std::unordered_map<uint32, PhysicsComponentInternal*> m_Components;
};
