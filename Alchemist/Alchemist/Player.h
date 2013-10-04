#pragma once

#include "Human.h"
#include "GrappleHook.h"
#include "MenuWindow.h"
#include "RawInputManager.h"

class Player : public Human
{
public:
	Player(void);
	Player(short ID);
	~Player(void);

	static int setInputManager(RawInputManager *);
	static void setCursorPos(double x, double y);

	virtual int initBullet();

	virtual int renderFrame(long time);
	virtual int update(long time);

private:	
	virtual int initGeom();

	GrappleHook hook;
	BodyPart * crosshair;

	static double cursorX;
	static double cursorY;
	static RawInputManager * inputManager;
};