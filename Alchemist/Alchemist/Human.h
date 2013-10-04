#ifndef HUMAN_H
#define HUMAN_H

#include "Character.h"
#include "Bullet.h"
#include "BodyPart.h"

#define WEP_HANDGUN 0
#define WEP_RIFLE	1
#define WEP_SHOTGUN	2

class Human : public Character
{
public:
	Human(void);
	Human(char *fileBase);
	Human(short ID);
	~Human(void);

	virtual int update(long time);
	virtual int renderFrame(long time);
	virtual int collide(GameObject *, const btVector3 * worldCollPos);
	virtual int initBullet();

	virtual void shoot(void);
	virtual void jump(void);

protected:
	virtual int initGeom();

	BodyPart * torso, * head, * frontArms, * backArms;
	float aimRotation;
	bool onGround;
	short weapon;

	D3DXVECTOR3 aimPos;
};

#endif