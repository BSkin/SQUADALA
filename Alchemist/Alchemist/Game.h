#pragma once

#pragma region Program Definitions
#define WIN32_LEAN_AND_MEAN

#ifndef WINVER                  
#define WINVER 0x0600
#endif

#ifndef _WIN32_WINNT            
#define _WIN32_WINNT 0x0600    
#endif

#ifndef _WIN32_WINDOWS          
#define _WIN32_WINDOWS 0x0410 
#endif

#define TITLE_SIZE 128
#pragma endregion

#pragma region Include and Using Statements
#include <windows.h>
#include <WinSock2.h>
//#include <Ws2tcpip.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>

#include <io.h>
#include <fcntl.h>
#include <fstream>

#include <d3d9.h>
#include <d3dx9.h>
#include "d3dx9math.h"
#include "D3DX10math.h"
#include "stdh.h"

//#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

#include "Packet.h"
#include "IndexedList.h"

#include "MenuWindow.h"
#include "AssetManager.h"
#include "AssetLoader.h"
#include "RawInputManager.h"

#include "GameObject.h"
#include "Player.h"
#include "Camera.h"

using std::cin;
using std::cout;
#pragma endregion

class Game
{
public:
	Game(void);
	Game(HINSTANCE hInstance, char* gameName);
	~Game(void);

	int init(void);
	int startGame(void);

private:
	static LRESULT CALLBACK globalWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
	#pragma region Runtime Functions
	int update(long time);
	int renderFrame(long time);
	int initGround(void);
	int cleanUp(void);
	int cleanBullet(void);
	#pragma endregion

	#pragma region Thread Functions
	static unsigned int WINAPI startLoader(LPVOID);
	static unsigned int WINAPI startListener(LPVOID);
	static unsigned int WINAPI hostThread(LPVOID);
	static unsigned int WINAPI clientThread(LPVOID);
	#pragma endregion

	#pragma region Static Declarations
	static IDirect3D9 * md3d;		
	static IDirect3DDevice9 * d3dDev;
	static RawInputManager * inputManager;
	static AssetManager * assetManager;
	static Packet packet;
	#pragma endregion

	int setd3dDev(IDirect3DDevice9 *d3dDev);
	
	void changeState(int targetState);
	short gameState;

	static short networkState, numClients;

	LPD3DXFONT font;

	#pragma region FPS
	//FPS methods
	void startTimer();
	double getTime();
	double getFrameTime();

	//FPS variables
	double countsPerSecond;
	INT64 CounterStart;
	int frameCount;
	int fps;
	INT64 frameTimeOld;
	double frameTime;
	#pragma endregion

	#pragma region Window Variables
	//Window Variables
	HINSTANCE hInstance;
	int fullScreen;
	static int wndWidth, wndHeight;
	HWND hwnd;
	char mGameName[TITLE_SIZE+1];
	bool console;
	static bool activeWindow;

	static D3DPRESENT_PARAMETERS * md3dpp; 

	D3DVIEWPORT9 viewPort;

	static int resizeWindow(int width, int height);
	#pragma endregion

	#pragma region Camera Variables
	Camera camera;

	D3DXMATRIX projMatrix;
	D3DXMATRIX viewMatrix;

	float nearPlane;
	float farPlane;
	float hFieldOfView;
	float vFieldOfView;
	float aspectRatio;
	#pragma endregion

	#pragma region Bullet Variables
	btAlignedObjectArray<btCollisionShape*>	collisionShapes;
	btDefaultCollisionConfiguration * collisionConfiguration;
	btCollisionDispatcher * dispatcher;
	btBroadphaseInterface * overlappingPairCache;
	btSequentialImpulseConstraintSolver * solver;
	btDiscreteDynamicsWorld * dynamicsWorld;
	//btIDebugDraw asdf;	<----------------------------TODO: Create an implementation which implements the drawLine method; 
	//														setDebugDrawer(asdf); dynamicsWorld->getDebugDrawer->setDebugMode(); dynamicsWorld->debugDrawWorld(); 
	#pragma endregion

	LPDIRECT3DSURFACE9 renderSurface, backBuffer;
	D3DSURFACE_DESC surfaceDesc;
	IDirect3DTexture9 * screenTexture;

	D3DXVECTOR3 sunDir;
	
	ID3DXEffect * screenEffect;
	IDirect3DVertexBuffer9 * screenVertexBuffer;
	D3DXHANDLE hMatrix;		//worldViewProj Matrix
	D3DXHANDLE hTexture;	//texture tex
	D3DXHANDLE hTime;
	D3DXHANDLE hTechnique;
	int loadScreenEffect(void);
	
	list<GameObject *> gameObjects;
	list<MenuWindow *> menuObjects;
	GameObject * floor;

	GameObject * player;
};