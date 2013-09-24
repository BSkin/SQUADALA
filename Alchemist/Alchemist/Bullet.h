#pragma once

#include "RigidObject.h"

class Bullet : public RigidObject
{
public:
	Bullet(char *fileBase, RigidObject * owner, long time);
	~Bullet();
	virtual int update(long time);
	virtual int collide(RigidObject *, const btVector3 * worldCollPos);

	virtual int initBullet();

	bool getDead();
private:
	RigidObject * owner;
	bool dead;
	long initTime;
	long lifeSpan;
};