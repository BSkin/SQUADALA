#pragma once

#include "RigidObject.h"

class Bullet : public RigidObject
{
public:
		Bullet(char *fileBase, RigidObject * owner);
		~Bullet();
		virtual int update(long time);
		virtual int collide(RigidObject *);

		bool getDead();
private:
	RigidObject * owner;
	bool dead;
};