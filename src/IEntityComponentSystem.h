
#pragma once

class IEntityComponentSystem
{
	friend class EntitySystem;

public:
    virtual ~IEntityComponentSystem() {}

    virtual bool Initialize() = 0;

    virtual void Shutdown() = 0;

protected:
	virtual void AttachComponent(uint32 ent) = 0;

	virtual void Update(uint32 dt) = 0;
};
