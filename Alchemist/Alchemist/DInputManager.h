#pragma once

#include <d3dx9.h>
#include "dinput.h"

class DInputManager
{
public:
	DInputManager(HINSTANCE hInst, HWND wndHandle);
	~DInputManager();
	
	static int setd3dDev(IDirect3DDevice9 *d3d_Dev);
	
	int getKeyboardKey(int DIK_CODE);
	D3DXVECTOR2 getMouseXY(void);
	int getMouseButton(int);
	
	void updateDevices(void);
private:
	static IDirect3DDevice9 *d3dDev;
	int init(HINSTANCE hInst, HWND wndHandle);
	LPDIRECTINPUT8 dInObj;
	LPDIRECTINPUTDEVICE8 keyDev;
	LPDIRECTINPUTDEVICE8 mouseDev;
	char keyBState[256];
	char mouseBState[4];
	DIMOUSESTATE mouseState;

	void updateKeyboard(void);
	void updateMouse(void);
	void updateGamepad(void);
	int keyboardKeys[256];
	int mouseKeys[4];
};