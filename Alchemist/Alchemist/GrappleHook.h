#pragma once

#include "Character.h"
#include "RigidObject.h"

#define IDLE		0
#define EXTENDING	1
#define LATCHED		2

#define MAX_DIST	300

class GrappleHook : public RigidObject
{
public:
	GrappleHook(Character *);
	~GrappleHook();

	void shoot(D3DXVECTOR3 pos, float rotation);
	void latch(GameObject * o);
	void unlatch();

	virtual int update(long time);
	virtual int renderFrame(long time);
	virtual int initBullet();

	virtual int collide(GameObject * other, const btVector3 * worldPos);

private:
	Character * owner;
	GameObject * attatchedObject;

	short state;
	GameObject line;
};