#ifndef DESTRUCTIBLE_OBJECT_H
#define DESTRUCTIBLE_OBJECT_H

#include "RigidObject.h"

class DestructibleObject : public RigidObject
{
public:
	DestructibleObject(void);
	DestructibleObject(char *fileBase);
	DestructibleObject(short ID);
	virtual ~DestructibleObject(void);

	virtual void modHealth(float);
};

#endif