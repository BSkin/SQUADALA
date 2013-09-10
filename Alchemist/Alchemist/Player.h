#pragma once

#include "Character.h"
#include "Bullet.h"
#include "BodyPart.h"
#include "MenuWindow.h"
#include "RawInputManager.h"

class Player : public Character
{
public:
	Player(void);
	Player(short ID);
	~Player(void);

	virtual int update(long time);
	virtual int renderFrame(long time);

	static int setInputManager(RawInputManager *);
	static void setCursorPos(D3DXVECTOR2);

private:
	virtual int initGeom();

	BodyPart * torso, * head, * frontArms, * backArms, * crosshair;
	float aimRotation;

	static D3DXVECTOR2 cursorPos;
	static RawInputManager * inputManager;
};