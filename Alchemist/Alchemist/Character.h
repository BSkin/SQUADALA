#pragma once

#include "RigidObject.h"
#include "ProjectileManager.h"

class Character : public RigidObject
{
public:
	Character(void);
	Character(char *fileBase);
	Character(short ID);
	~Character(void);
	void moveUp(void);
	void moveDown(void);
	void moveLeft(void);
	void moveRight(void);

	virtual int update(long time);
	virtual int renderFrame(long time);

	static int setProjectileManager(ProjectileManager *);

protected:
	float speed;
	D3DXVECTOR3 direction;
	static ProjectileManager * projectileManager;
};