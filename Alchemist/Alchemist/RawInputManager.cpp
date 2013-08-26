#include "RawInputManager.h"

RawInputManager::RawInputManager() : cam(0), mousePosition(0.0f, 0.0f), mouseX(0.0f), mouseY(0.0f)
{
	for (int i = 0; i < keyboardSize; i++) 
	{
		keyDown[i] = false;
		keyState[i] = 0;
	}
	for (int i = 0; i < mouseSize; i++) 
	{
		mouseButtonDown[i] = false;
		mouseButtonState[i] = 0;
	}
}

RawInputManager::~RawInputManager()
{

}

void RawInputManager::updateRawMouse(RAWINPUT* raw, LPARAM lParam)
{	
    if (raw->header.dwType == RIM_TYPEMOUSE) 
    {
		/*    raw->data.mouse.usFlags, 
            raw->data.mouse.ulButtons, 
            raw->data.mouse.usButtonFlags, 
            raw->data.mouse.usButtonData, 
            raw->data.mouse.ulRawButtons, 
            raw->data.mouse.lLastX, 
            raw->data.mouse.lLastY, 
            raw->data.mouse.ulExtraInformation);
		*/

		if (raw->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_DOWN)
			mouseButtonDown[0] = true;
		else if (raw->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_UP)
			mouseButtonDown[0] = false;

        if (raw->data.mouse.ulButtons & RI_MOUSE_MIDDLE_BUTTON_DOWN)
			mouseButtonDown[1] = true;
		else if (raw->data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_DOWN)
			mouseButtonDown[1] = false;

		mouseX = raw->data.mouse.lLastX;
		mouseY = raw->data.mouse.lLastY;

		cam->turn(mouseX, mouseY);

		/*
		mouseMov = D3DXVECTOR2(
			(float)raw->data.mouse.lLastX,
			(float)raw->data.mouse.lLastY);
		*/

		POINT tempPos;
		GetCursorPos(&tempPos);
		mousePosition = D3DXVECTOR2((float)tempPos.x, (float)tempPos.y);
    }
}

void RawInputManager::zeroMouseMov() { mouseX = 0.0; mouseY = 0.0; }

void RawInputManager::updateKeyboard()
{
	for (int i = 0; i < keyboardSize; i++)
	{
		if (GetAsyncKeyState(i) == 0) keyDown[i] = false;
		else keyDown[i] = true;
	}
}

void RawInputManager::update()
{
	updateKeyboard();

	for (int i = 0; i < keyboardSize; i++)
	{
		if (keyDown[i]) keyState[i]++;
		else keyState[i] = 0;
	}
	for (int i = 0; i < mouseSize; i++)
	{
		if (mouseButtonDown[i]) mouseButtonState[i]++;
		else mouseButtonState[i] = 0;
	}
}

int RawInputManager::getKey(USHORT keyCode) { return keyState[keyCode]; }
int RawInputManager::getMouseKey(short index) { return mouseButtonState[index]; }
D3DXVECTOR2 RawInputManager::getMousePos() { return mousePosition; }
long RawInputManager::getMouseX() { return mouseX; }
long RawInputManager::getMouseY() { return mouseY; }
void RawInputManager::setCamera(Camera * camera) { cam = camera; }