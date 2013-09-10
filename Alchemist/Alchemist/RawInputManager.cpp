#include "RawInputManager.h"

float * RawInputManager::wndWidth = NULL;
float * RawInputManager::wndHeight = NULL;
HWND * RawInputManager::hwnd = NULL;

RawInputManager::RawInputManager() : cam(0), mousePosition(0.0f, 0.0f), mouseX(0.0f), mouseY(0.0f)
{
	for (int i = 0; i < keyboardSize; i++) 
	{
		keyDownState[i] = false;
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

        if (raw->data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_DOWN)
			mouseButtonDown[1] = true;
		else if (raw->data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_UP)
			mouseButtonDown[1] = false;

		mouseX = raw->data.mouse.lLastX;
		mouseY = raw->data.mouse.lLastY;

		cam->turn(mouseX, mouseY);

		/*
		mouseMov = D3DXVECTOR2(
			(float)raw->data.mouse.lLastX,
			(float)raw->data.mouse.lLastY);
		*/

		POINT cp;
		GetCursorPos(&cp);

		RECT rekt;
		GetWindowRect(*hwnd, &rekt);

		cp.x -= rekt.left + *wndWidth*0.5;
		cp.y -= rekt.top;
		cp.y = *wndHeight - cp.y - *wndHeight*0.5;
		mousePosition = D3DXVECTOR2((float)cp.x, (float)cp.y);
    }
}

void RawInputManager::zeroMouseMov() { mouseX = 0.0; mouseY = 0.0; }

void RawInputManager::updateKeyboard()
{
	for (int i = 0; i < keyboardSize; i++)
	{
		if (GetAsyncKeyState(i) == 0) keyDownState[i] = false;
		else keyDownState[i] = true;
	}
}

void RawInputManager::update()
{
	updateKeyboard();

	for (int i = 0; i < keyboardSize; i++)
	{
		if (keyDownState[i]) keyState[i]++;
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

void RawInputManager::setWindowVariables(float * x, float * y, HWND * h) { wndWidth = x; wndHeight = y; hwnd = h; }

bool RawInputManager::keyPress(USHORT keyCode)
{
	return keyState[keyCode] == 1;
}

bool RawInputManager::keyDown(USHORT keyCode)
{
	return keyDownState[keyCode];
}

bool RawInputManager::keyUp(USHORT keyCode)
{
	return !keyDownState[keyCode];
}

bool RawInputManager::getMouseKeyPress(short keyCode)
{
	return mouseButtonState[keyCode] == 1;
}

bool RawInputManager::getMouseKeyDown(short keyCode)
{
	return mouseButtonDown[keyCode];
}

bool RawInputManager::getMouseKeyUp(short keyCode)
{
	return !mouseButtonDown[keyCode];
}