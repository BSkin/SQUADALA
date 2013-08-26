#pragma once

//#include <Windows.h>
#include <d3dx9.h>
#include "Camera.h"

#define keyboardSize 255
#define mouseSize 2 

class RawInputManager
{
public:
	RawInputManager();
	~RawInputManager();
	void updateRawMouse(RAWINPUT* raw, LPARAM lParam);
	void updateKeyboard();
	void update();

	int getKey(USHORT keyCode);
	int getMouseKey(short keyCode);
	D3DXVECTOR2 getMousePos();
	long getMouseX();
	long getMouseY();

	void zeroMouseMov();

	void setCamera(Camera * cam);

private:
	Camera * cam;

	bool keyDown[keyboardSize];
	int keyState[keyboardSize];

	bool mouseButtonDown[mouseSize];
	int mouseButtonState[mouseSize];
	D3DXVECTOR2 mousePosition;
	long mouseX;
	long mouseY;
};