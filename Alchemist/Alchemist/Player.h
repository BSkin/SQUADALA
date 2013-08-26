#pragma once

#include "GameObject.h"
#include "RawInputManager.h"
#include "Camera.h"

class Player : public GameObject
{
public:
	Player(void);
	Player(char *fileBase);
	Player(short ID);
	~Player(void);

	virtual int update(long time);
	virtual int renderFrame(long time);

	static int setInputManager(RawInputManager *);
	static int setCamera(Camera *);

private:
	virtual int initBullet();
	virtual int initGeom();

	static RawInputManager * inputManager;
	static Camera * camera;
};