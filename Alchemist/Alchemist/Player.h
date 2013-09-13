#pragma once

#include "Character.h"
#include "Bullet.h"
#include "BodyPart.h"
#include "MenuWindow.h"
#include "RawInputManager.h"

#define WEP_HANDGUN 0
#define WEP_RIFLE	1
#define WEP_SHOTGUN	2

class Player : public Character
{
public:
	Player(void);
	Player(short ID);
	~Player(void);

	virtual int update(long time);
	virtual int renderFrame(long time);
	virtual int collide(RigidObject *, const btVector3 * worldCollPos);

	static int setInputManager(RawInputManager *);
	static void setCursorPos(double x, double y);

private:
	virtual int initGeom();

	BodyPart * torso, * head, * frontArms, * backArms, * crosshair;
	float aimRotation;
	bool onGround;
	short weapon;

	static double cursorX;
	static double cursorY;
	static RawInputManager * inputManager;
};