
#pragma once

#include <stdint.h>

class IEntityComponentSystem
{
	friend class EntitySystem;

public:
    virtual ~IEntityComponentSystem() {}

    virtual bool Initialize() = 0;

    virtual void Shutdown() = 0;

protected:
	virtual void AttachComponent(uint32_t ent) = 0;

	virtual void Update(uint32_t dt) = 0;
};
