#include "DestructibleObject.h"

DestructibleObject::DestructibleObject(void) : RigidObject() 
{
	health = 25;
}

DestructibleObject::DestructibleObject(char *fileBase) : RigidObject(fileBase) 
{
	health = 25;
}

DestructibleObject::DestructibleObject(short ID) : RigidObject(ID) 
{
	health = 25;
}

DestructibleObject::~DestructibleObject() {}

void DestructibleObject::modHealth(float x)
{
	health += x;
}