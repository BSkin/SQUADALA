#pragma once

#include "RigidObject.h"

class Bullet : public RigidObject
{
public:
	Bullet(char *fileBase, RigidObject * owner, long time);
	virtual ~Bullet();
	virtual int update(long time);
	virtual int collide(GameObject *, const btVector3 * worldCollPos);

	virtual int initBullet();

	bool getDead();
private:
	RigidObject * owner;
	float damage;
};