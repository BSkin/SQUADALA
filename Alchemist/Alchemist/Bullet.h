#pragma once

#include "RigidObject.h"

class Bullet : public RigidObject
{
public:
		Bullet(char *fileBase);
		virtual int update(long time);
};