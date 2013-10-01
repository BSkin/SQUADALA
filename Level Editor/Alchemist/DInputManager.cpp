#include "DInputManager.h"

IDirect3DDevice9 * DInputManager::d3dDev = NULL;

DInputManager::DInputManager(HINSTANCE hInst, HWND wndHandle)
{
	int result = init(hInst, wndHandle);
	for (int x = 0; x < 256; x++) keyboardKeys[x] = 0;
	for (int x = 0; x < 4; x++) mouseBState[x] = 0;
}

DInputManager::~DInputManager()
{

}

#pragma region Setup Functions
int DInputManager::init(HINSTANCE hInst, HWND wndHandle)
{
	DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dInObj, NULL);
	
	dInObj->CreateDevice(GUID_SysKeyboard, &keyDev, NULL);
	keyDev->SetDataFormat(&c_dfDIKeyboard);
	keyDev->SetCooperativeLevel(wndHandle, DISCL_FOREGROUND|DISCL_NONEXCLUSIVE);
	keyDev->Acquire();

	

	dInObj->CreateDevice(GUID_SysMouse, &mouseDev, NULL);
	mouseDev->SetDataFormat(&c_dfDIMouse);
	mouseDev->SetCooperativeLevel(wndHandle, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	mouseDev->Acquire();

	return 0;
}

int DInputManager::setd3dDev(IDirect3DDevice9 *d3d_Dev)
{
	if (d3dDev == NULL)
		d3dDev = d3d_Dev;
	else
		return 1; //device is already set, cannot be re set

	return 0; //normal exit status
}
#pragma endregion

#pragma region Get Functions
int DInputManager::getKeyboardKey(int DIK_CODE) { return keyboardKeys[DIK_CODE]; }

int DInputManager::getMouseButton(int i) { return mouseBState[i]; }

D3DXVECTOR2 DInputManager::getMouseXY(void)
{
	return D3DXVECTOR2(mouseState.lX, mouseState.lY);
}

#pragma endregion

#pragma region Update Functions
void DInputManager::updateKeyboard(void)
{
	int hr = keyDev->GetDeviceState(sizeof(keyBState),(LPVOID)&keyBState);
	if (FAILED (hr)) keyDev->Acquire();

	for (int i = 0; i < 256; i++)
	{
		if (keyBState[i]/128) 
			keyboardKeys[i]++;
		else keyboardKeys[i] = 0;
	}
}

void DInputManager::updateMouse(void)
{
	int hr = mouseDev->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouseState);
	if (FAILED(hr)) mouseDev->Acquire();

	for (int i = 0; i < 4; i++)
	{
		if (mouseState.rgbButtons[i]/128) 
			mouseBState[i]++;
		else mouseBState[i] = 0;
	}
}

void DInputManager::updateGamepad(void)
{

}

void DInputManager::updateDevices(void)
{
	updateKeyboard();
	updateMouse();
	//updateGamepad();
}
#pragma endregion
