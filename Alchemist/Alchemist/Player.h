#pragma once

#include "Character.h"
#include "RawInputManager.h"

class Player : public Character
{
public:
	Player(void);
	Player(char *fileBase);
	Player(short ID);
	~Player(void);

	virtual int update(long time);
	virtual int renderFrame(long time);

	static int setInputManager(RawInputManager *);

private:
	virtual int initGeom();

	static RawInputManager * inputManager;
};