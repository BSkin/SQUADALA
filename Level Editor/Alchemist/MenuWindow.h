#pragma once

#include "GameObject.h"
#include "Quad.h"
#include "Camera.h"

#define TOP_LEFT	0
#define TOP_RIGHT	1
#define BOT_LEFT	2
#define BOT_RIGHT	3
#define MID_TOP		4
#define MID_BOT		5
#define MID_LEFT	6
#define MID_RIGHT	7
#define MID			8

class MenuWindow : public GameObject
{
public:
	MenuWindow();
	MenuWindow(const char *dir);
	~MenuWindow();

	int init(const char *dir);

	virtual void setSizePercentage(float, float);	//size in percentage of screen
	void setPosition(float, float);
	virtual void setPositionPercent(float, float);
	virtual void setSize(int, int);
	void setPosition(int x, int y, int anchor);
	void modPosition(float, float);	
	void modPosition(int x, int y);

	virtual int renderFrame(long time);
	static void setStatics(Camera *, int, int);
private:
	static float wndWidth, wndHeight;
	static Camera * camera;
	
	float screenX, screenY;

	void updatePosition();
};