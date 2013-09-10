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

	bool keyPress(USHORT keyCode);
	bool keyDown(USHORT keyCode);
	bool keyUp(USHORT keyCode);

	bool getMouseKeyPress(short keyCode);
	bool getMouseKeyDown(short keyCode);
	bool getMouseKeyUp(short keyCode);

	D3DXVECTOR2 getMousePos();
	long getMouseX();
	long getMouseY();

	void zeroMouseMov();

	void setCamera(Camera * cam);
	
	static void setWindowVariables(float *, float *, HWND *);

private:
	int getKey(USHORT keyCode);
	int getMouseKey(short keyCode);

	Camera * cam;

	bool keyDownState[keyboardSize];
	int keyState[keyboardSize];

	bool mouseButtonDown[mouseSize];
	int mouseButtonState[mouseSize];
	D3DXVECTOR2 mousePosition;
	long mouseX;
	long mouseY;

	static float * wndWidth;
	static float * wndHeight;
	static HWND * hwnd;
};