#include "Game.h"

#pragma region Game Constants
#define WND_WIDTH 1280
#define WND_HEIGHT 720
#define FRAMES_PER_SECOND 60

#define START_MENU	0
#define PAUSE_MENU	1
#define IN_GAME		2

#define sideWidth	70

#pragma endregion

#pragma region Initialize Statics
IDirect3DDevice9 * Game::d3dDev = NULL;
IDirect3D9 * Game::md3d = NULL;
AssetManager * Game::assetManager = NULL;
RawInputManager * Game::inputManager = NULL;
float Game::wndWidth = 0;
float Game::wndHeight = 0;
short Game::networkState = 0;
short Game::numClients = 0;
bool Game::activeWindow = true;
//LPDIRECT3DSURFACE9 Game::renderSurface = NULL;
//LPDIRECT3DSURFACE9 Game::backBuffer = NULL;
D3DPRESENT_PARAMETERS * Game::md3dpp = NULL;
#pragma endregion

LRESULT Game::globalWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int rc = 1;
	switch (msg)
	{
		case WM_INPUT:
		{
			if (true) //in game
			{
				UINT dwSize = 0;

				GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, 
                    sizeof(RAWINPUTHEADER));
				LPBYTE lpb = new BYTE[dwSize];
				if (lpb == NULL) return 0;  

				GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

				RAWINPUT* raw = (RAWINPUT*)lpb;
			
				inputManager->updateRawMouse(raw, lParam);

				delete[] lpb;
			}
			return 0;
		}
		case WM_ACTIVATEAPP:
		{
			activeWindow = (bool)wParam;
			return 0;
		}
		case WM_SIZE:
		{
			RECT rect;
			if(GetWindowRect(hwnd, &rect))
			{
				int width = rect.right - rect.left;
				int height = rect.bottom - rect.top;
				resizeWindow(width, height);
			}
			return 0;
		}
		case WM_CLOSE:
		{
			DestroyWindow(hwnd);
			return 0;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			// Handle any messages the switch statement didn't
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

Game::Game(HINSTANCE hInst, char* gameName) : countsPerSecond(0.0), CounterStart(0), frameCount(0), fps(0), frameTimeOld(0), frameTime(0.0), 
												menuObjects(), gameObjects(), tabObjects(), sunDir(1,-1,0), screenEffect(0), tab(false),
												renderSurface(NULL), backBuffer(NULL), 
												selectedTexture(NULL), selectedSideBar(NULL), cursor(NULL),
												p1(0,0,0), p2(0,0,0), selectedRect(0)
{ 
	fullScreen = false;
	console = false;

	SetCurrentDirectory("..\\..\\Alchemist\\Alchemist");

	size = 100;

	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < numTextures/10+1; y++)
		{
			if (x+y*10 < numTextures)
			{
				textures[x][y] = 0;
				textureStrings[x][y] = "Assets\\UI\\light grey.png";;
			}
		}
	}

	textureStrings[0][0] = "Assets\\UI\\black.png";
	textureStrings[1][0] = "Assets\\UI\\blue.png";
	textureStrings[2][0] = "Assets\\UI\\brown.png";
	textureStrings[3][0] = "Assets\\UI\\dark blue.png";
	textureStrings[4][0] = "Assets\\UI\\dark grey.png";
	textureStrings[5][0] = "Assets\\UI\\green.png";
	textureStrings[6][0] = "Assets\\UI\\ligt grey.png";
	textureStrings[7][0] = "Assets\\UI\\mahogany.png";
	textureStrings[8][0] = "Assets\\UI\\orange.png";
	textureStrings[9][0] = "Assets\\UI\\pink.png";

	textureStrings[0][1] = "Assets\\UI\\purple.png";
	textureStrings[1][1] = "Assets\\UI\\red.png";
	textureStrings[2][1] = "Assets\\UI\\violet.png";
	textureStrings[3][1] = "Assets\\UI\\white.png";
	textureStrings[4][1] = "Assets\\UI\\yellow.png";
	textureStrings[5][1] = "Assets\\crate.png";
	textureStrings[6][1] = "Assets\\Face.png";
	textureStrings[7][1] = "Assets\\terr.png";
	textureStrings[8][1] = "Assets\\pixeltest.png";
	textureStrings[9][1] = "Assets\\HealthTemp.png";

	textureStrings[0][2] = "Assets\\reds.png";
	textureStrings[1][2] = "Assets\\testhud.png";
	textureStrings[2][2] = "Assets\\transtest.png";

	hInstance = hInst;
	wndWidth = WND_WIDTH;
	wndHeight = WND_HEIGHT;
	fps = FRAMES_PER_SECOND;
	hwnd = NULL;
	if (mGameName == NULL) this->mGameName[0] = '\0';
	else strncpy(this->mGameName, gameName,TITLE_SIZE);
	srand(time(NULL));
}

Game::~Game(void) {}

int Game::init(void)
{
	int ec = 0;

	#pragma region Initialize Window
	char *winClass = "gameAppClass";
	WNDCLASS wc;

	//ShowCursor(false);

	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= globalWndProc;
	wc.hInstance		= hInstance;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName	= winClass;
	wc.hIcon			= LoadIcon(0, IDI_APPLICATION);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= winClass;

	ec = RegisterClass(&wc);

	ec = 0;  

	if (fullScreen)
	{
		const HWND hDesktop = GetDesktopWindow();
		RECT desktop;
		GetWindowRect(hDesktop, &desktop);
		wndWidth = desktop.right;
		wndHeight = desktop.bottom;
	}

	hwnd = CreateWindowEx(NULL,
		winClass, 
		mGameName, 
		WS_POPUP|WS_VISIBLE,//WS_OVERLAPPEDWINDOW | WS_VISIBLE,//WS_POPUP, 
		0, 0, 
		wndWidth, wndHeight, 
		NULL, 
		NULL, 
		hInstance, 
		NULL);
		
    if (hwnd == NULL) {
	   ec = GetLastError();
	   ec = 1;
	}else {
	   ShowWindow(hwnd, SW_SHOW);
	}
	#pragma endregion

	#pragma region Initialize Console
	if (console) 
	{
		AllocConsole();

		long lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
		int hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		FILE *fp = _fdopen( hConHandle, "w" );
		*stdout = *fp;
		setvbuf( stdout, NULL, _IONBF, 0 );
	}
	#pragma endregion

	#pragma region Initialize D3D

	if (md3d == NULL) {
		md3d = Direct3DCreate9(D3D_SDK_VERSION);
		if (md3d == NULL) {
			ec = 1;
			return(ec);
		}
	}

	if (ec == 1) return ec;

	md3dpp = new D3DPRESENT_PARAMETERS();
	ZeroMemory(md3dpp, sizeof(D3DPRESENT_PARAMETERS));
	md3dpp->BackBufferCount = 1;
	md3dpp->BackBufferWidth = wndWidth;
	md3dpp->BackBufferHeight = wndHeight;

	md3dpp->MultiSampleType = D3DMULTISAMPLE_NONE;
	md3dpp->MultiSampleQuality = 0;

	md3dpp->SwapEffect = D3DSWAPEFFECT_DISCARD;
	md3dpp->Windowed = !fullScreen;
	if (fullScreen)
		wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

	md3dpp->hDeviceWindow = hwnd;
	md3dpp->Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	
	md3dpp->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	md3dpp->PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	//md3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	//md3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	md3dpp->BackBufferFormat = D3DFMT_X8R8G8B8;//D3DFMT_R5G6B5;
	md3dpp->EnableAutoDepthStencil = TRUE;
	md3dpp->AutoDepthStencilFormat = D3DFMT_D16;

	ec = md3d->CreateDevice(D3DADAPTER_DEFAULT,	
							 D3DDEVTYPE_HAL,
							 hwnd,
							 D3DCREATE_SOFTWARE_VERTEXPROCESSING,
							 //D3DCREATE_HARDWARE_VERTEXPROCESSING,
							 md3dpp,
							 &d3dDev);
							 
	std::cout << "D3D Device Successfully Created\n";

    d3dDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW  );
    d3dDev->SetRenderState( D3DRS_ZENABLE, TRUE );
	d3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );

	//Alpha Blending
	d3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	d3dDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	d3dDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	d3dDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	d3dDev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	d3dDev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	
	if (ec != S_OK) {
		if (md3d != NULL) 
		{
			md3d->Release(); 
			(md3d) = NULL;
		}
		ec = 1;
	}

	assetManager = new AssetManager();

	D3DXCreateFont(d3dDev,
						15, // height of font
						0,	// use the default width
						FW_NORMAL,	// normal font weight
						1, // no Mipmap
						FALSE, // italic
						DEFAULT_CHARSET,
						OUT_DEFAULT_PRECIS,
						DEFAULT_QUALITY,
						DEFAULT_PITCH ||FF_DONTCARE,
						"Arial",
						&font);  

	viewPort.Height = wndHeight;
	viewPort.Width = wndWidth;
	viewPort.X = 0;
	viewPort.Y = 0;
	viewPort.MinZ = 0;
	viewPort.MaxZ  = 1;

	d3dDev->SetViewport(&viewPort);
	d3dDev->GetRenderTarget(0, &backBuffer);
	backBuffer->GetDesc(&surfaceDesc);

	//d3dDev->CreateRenderTarget(surfaceDesc.Width, surfaceDesc.Height, surfaceDesc.Format, 
    //                             surfaceDesc.MultiSampleType, surfaceDesc.MultiSampleQuality,
    //                              false, &renderSurface, NULL);
		
	d3dDev->CreateTexture(surfaceDesc.Width, surfaceDesc.Height, 1, D3DUSAGE_RENDERTARGET,
						D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &screenTexture, NULL);
	
	screenTexture->GetSurfaceLevel(0, &renderSurface);

	d3dDev->CreateVertexBuffer(sizeof(VERTEX) * 4, NULL, D3DFVF_VERTEX, D3DPOOL_MANAGED, &screenVertexBuffer, NULL);
	#pragma endregion

	#pragma region Initialize Input
	RAWINPUTDEVICE Rid;

	Rid.usUsagePage = 0x01 ;  // magic numbers
	Rid.usUsage = 0x02 ;      // magically means mouse
	Rid.dwFlags = RIDEV_INPUTSINK ; // (use this if you DO NOT WANT to capture mouse)
	Rid.hwndTarget = hwnd ;

	//Rid[1].usUsagePage = 0x01 ;  // magic numbers
	//Rid[1].usUsage = 0x06 ;      // magically means keyboard
	//Rid[1].dwFlags = 0 ;         // use RIDEV_NOHOTKEYS for no winkey
	//Rid[1].hwndTarget = hwnd ;

	RegisterRawInputDevices(&Rid, 1, sizeof(Rid));
	
	inputManager = new RawInputManager();
	inputManager->setCamera(&camera);

	std::cout << "Raw Input System Initialized\n";
	#pragma endregion

	#pragma region Set Static References
	setd3dDev(d3dDev);
	AssetManager::setd3dDev(d3dDev);
	AssetLoader::setd3dDev(d3dDev);
	RawInputManager::setWindowVariables(&wndWidth, &wndHeight, &hwnd);
	Quad::setD3dDev(d3dDev);
	Quad::setAssetManager(assetManager);
	GameObject::setd3dDev(d3dDev);
	GameObject::setAssetManager(assetManager);
	GameObject::setCamera(&camera);
	MenuWindow::setStatics(&camera, wndWidth, wndHeight);
	std::cout << "Static References Set\n";
	#pragma endregion

	loadScreenEffect();

	_beginthreadex(NULL, 0, startLoader, NULL, NULL, NULL);		//Asset Loader

	return 0;
}

int Game::setd3dDev(IDirect3DDevice9 *d3dDev)
{
	if (d3dDev == NULL) 
	{
		d3dDev = d3dDev;
		return 0;
	} 
	return 1;
}

int Game::startGame(void)
{
	int rc = 0;
    MSG msg;

	static DWORD LastRenderTime = GetTickCount();
	DWORD ElapsedTime = 0;
	DWORD TimePerFrame = 1000 / fps;
	DWORD CurrentTime = 1;

	hFieldOfView = 100.0f;
	vFieldOfView = hFieldOfView * WND_HEIGHT / WND_WIDTH;
	camera.setDimensions(wndWidth, wndHeight);
	camera.setProj(0.1f, 5000.0, D3DXToRadian(vFieldOfView), (WND_WIDTH*1.0f)/(WND_HEIGHT*1.0f));
	
	static long time = 1;
	
	int i = 0;
	
	changeState(START_MENU);
	
	std::cout << "Starting Game Loop\n";

	#pragma region Game Loop
	while(TRUE)
    {
        // Check to see if any messages are waiting in the queue
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
			if( msg.message == WM_QUIT )
			{
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
        }
		else
		{
			#pragma region Framerate
			frameCount++;
			if(getTime() > 1.0f)
			{
				fps = frameCount;
				frameCount = 0;
				startTimer();
			}	
			frameTime = getFrameTime();
			#pragma endregion

			if (activeWindow)
			{
				int rc = update(time);
				if (rc < 0) break;

				renderFrame(time);
				time++;
			}
        
			CurrentTime = GetTickCount();
			ElapsedTime = CurrentTime - LastRenderTime;
			if (ElapsedTime < TimePerFrame) { Sleep(TimePerFrame - ElapsedTime); }
			LastRenderTime = GetTickCount();
		}
    }
	#pragma endregion

	cleanUp();

    return msg.wParam;
}

int Game::update(long time)
{
	inputManager->update();
	//camera.modPosition(1,0,0);

	#pragma region Start Menu
	if (gameState == START_MENU)
	{
		if (inputManager->keyPress(VK_RETURN))
		{
			changeState(IN_GAME);
			return 0;
		}
		if (inputManager->keyPress(VK_ESCAPE)) return -1;
	}
	#pragma endregion

	#pragma region In Game
	else if (gameState == IN_GAME)
	{
		//SetCursorPos(wndWidth/2, wndHeight/2);

		for (int x = 0; x < 10; x++)
		{
			for (int y = 0; y < numTextures/10+1; y++)
			{
				assetManager->getTexture(textureStrings[x][y].c_str(), &textures[x][y]);
			}
		}

		if (inputManager->keyPress(VK_ESCAPE)) 
		{
			changeState(START_MENU);
			return 0;
		}

		if (inputManager->keyDown('W')) camera.modPosition(0,0.5,0);
		if (inputManager->keyDown('S')) camera.modPosition(0,-0.5,0);
		if (inputManager->keyDown('A')) camera.modPosition(-0.5,0,0);
		if (inputManager->keyDown('D')) camera.modPosition(0.5,0,0);
		if (inputManager->keyPress('Z') && inputManager->keyDown(VK_LCONTROL))
		{
			if (gameObjects.size() > 1)
			{
				delete gameObjects.back();
				gameObjects.pop_back();
			}
		}
		if (inputManager->keyPress(VK_TAB)) tab = !tab;
		
		if (inputManager->getMousePos().x < (-wndWidth/2 + sideWidth + 30) )
		{
			if (inputManager->mouseKeyPress(0))
			{
				int x = getSelectedSideBar();
				selectedTexture = sideBar[x]->getTexture();
			}

		}
		else
		{
			if (tab)
			{
				short mouseWheel = inputManager->getMouseWheel();
				if (mouseWheel)
				{
					//mouseWheel/=abs(mouseWheel);
					changePage(mouseWheel);
				}

				if (inputManager->mouseKeyPress(0))
				{
					POINT p = getSelectedTabBar();
					p.y += pageNumber*6;
					if (p.x != -1 && p.y != -1 && textures[p.x][p.y]) 
					{
						selectedTexture = textures[p.x][p.y];

						if (inputManager->keyDown('1')) 
						{
							sideBar[0]->setTexture(textures[p.x][p.y]);
							selectedSideBar->setPosition(sideWidth, -1, TOP_LEFT);
						}

						if (inputManager->keyDown('2')) 
						{
							sideBar[1]->setTexture(textures[p.x][p.y]);
							selectedSideBar->setPosition(sideWidth, -sideWidth-3, TOP_LEFT);
						}

						if (inputManager->keyDown('3')) 
						{
							sideBar[2]->setTexture(textures[p.x][p.y]);
							selectedSideBar->setPosition(sideWidth, -sideWidth*2-5, TOP_LEFT);
						}

						if (inputManager->keyDown('4')) 
						{
							sideBar[3]->setTexture(textures[p.x][p.y]);
							selectedSideBar->setPosition(sideWidth, -sideWidth*3-7, TOP_LEFT);
						}

						if (inputManager->keyDown('5')) 
						{
							sideBar[4]->setTexture(textures[p.x][p.y]);
							selectedSideBar->setPosition(sideWidth, -sideWidth*4-9, TOP_LEFT);
						}

						if (inputManager->keyDown('6')) 
						{
							sideBar[5]->setTexture(textures[p.x][p.y]);
							selectedSideBar->setPosition(sideWidth, -sideWidth*5-11, TOP_LEFT);
						}

						if (inputManager->keyDown('7')) 
						{
							sideBar[6]->setTexture(textures[p.x][p.y]);
							selectedSideBar->setPosition(sideWidth, -sideWidth*6-13, TOP_LEFT);
						}

						if (inputManager->keyDown('8')) 
						{
							sideBar[7]->setTexture(textures[p.x][p.y]);
							selectedSideBar->setPosition(sideWidth, -sideWidth*7-15, TOP_LEFT);
						}

						if (inputManager->keyDown('9')) 
						{
							sideBar[8]->setTexture(textures[p.x][p.y]);
							selectedSideBar->setPosition(sideWidth, -sideWidth*8-17, TOP_LEFT);
						}

						if (inputManager->keyDown('0')) 
						{
							sideBar[9]->setTexture(textures[p.x][p.y]);
							selectedSideBar->setPosition(sideWidth, -sideWidth*9-19, TOP_LEFT);
						}
					}
				}
			}
			else
			{
				if (inputManager->keyPress('1'))
				{
					selectedTexture = sideBar[0]->getTexture();
					selectedSideBar->setPosition(sideWidth, -1, TOP_LEFT);
				}

				if (inputManager->keyPress('2'))
				{
					selectedTexture = sideBar[1]->getTexture();
					selectedSideBar->setPosition(sideWidth, -sideWidth-3, TOP_LEFT);
				}

				if (inputManager->keyPress('3'))
				{
					selectedTexture = sideBar[2]->getTexture();
					selectedSideBar->setPosition(sideWidth, -sideWidth*2-5, TOP_LEFT);
				}

				if (inputManager->keyPress('4'))
				{
					selectedTexture = sideBar[3]->getTexture();
					selectedSideBar->setPosition(sideWidth, -sideWidth*3-7, TOP_LEFT);
				}

				if (inputManager->keyPress('5'))
				{
					selectedTexture = sideBar[4]->getTexture();
					selectedSideBar->setPosition(sideWidth, -sideWidth*4-9, TOP_LEFT);
				}

				if (inputManager->keyPress('6'))
				{
					selectedTexture = sideBar[5]->getTexture();
					selectedSideBar->setPosition(sideWidth, -sideWidth*5-11, TOP_LEFT);
				}

				if (inputManager->keyPress('7'))
				{
					selectedTexture = sideBar[6]->getTexture();
					selectedSideBar->setPosition(sideWidth, -sideWidth*6-13, TOP_LEFT);
				}

				if (inputManager->keyPress('8'))
				{
					selectedTexture = sideBar[7]->getTexture();
					selectedSideBar->setPosition(sideWidth, -sideWidth*7-15, TOP_LEFT);
				}

				if (inputManager->keyPress('9'))
				{
					selectedTexture = sideBar[8]->getTexture();
					selectedSideBar->setPosition(sideWidth, -sideWidth*8-17, TOP_LEFT);
				}

				if (inputManager->keyPress('0'))
				{
					selectedTexture = sideBar[9]->getTexture();
					selectedSideBar->setPosition(sideWidth, -sideWidth*9-19, TOP_LEFT);
				}

				if (inputManager->keyPress(VK_UP)) size += 50;
				if (inputManager->keyPress(VK_DOWN)) size -= 50;

				/*if (inputManager->mouseKeyPress(0) && selectedTexture != NULL)
				{
					GameObject * o = new GameObject(selectedTexture);
					D3DXVECTOR3 pos = D3DXVECTOR3(
											inputManager->getMousePos().x + camera.getPosition().x*100,
											inputManager->getMousePos().y + camera.getPosition().y*100, 
											0);
					o->setPosition(pos);
					o->setSize(size, size);
					gameObjects.push_back(o);
				}*/

				if (inputManager->mouseKeyPress(0)) p1 = D3DXVECTOR3(
																inputManager->getMousePos().x + camera.getPosition().x*100, 
																inputManager->getMousePos().y + camera.getPosition().y*100, 
																-1);
				if (inputManager->mouseKeyDown(0)) 
				{
					p2 = D3DXVECTOR3(
								inputManager->getMousePos().x + camera.getPosition().x*100, 
								inputManager->getMousePos().y + camera.getPosition().y*100, 
								-1);

					float width = abs(p2.x - p1.x);
					float height = abs(p2.y - p1.y);
					D3DXVECTOR3 pos = D3DXVECTOR3(
											(p2.x + p1.x) / 2,
											(p2.y + p1.y) / 2,
											-10);

					selectedRect->setSize(width, height);
					selectedRect->setPosition(pos);
				}
				else if (inputManager->mouseKeyRelease(0))
				{
					float width = abs(p2.x - p1.x);
					float height = abs(p2.y - p1.y);
					D3DXVECTOR3 pos = D3DXVECTOR3(
											(p2.x + p1.x) / 2,
											(p2.y + p1.y) / 2,
											-10);

					if (selectedTexture)
					{
						GameObject * o = new GameObject(selectedTexture);
						o->setSize(width, height);
						o->setPosition(pos.x, pos.y, 0);
						gameObjects.push_back(o);
					}
				}
				else
				{
					selectedRect->setSize(0,0);
					//selectedRect->setPosition(inputManager->getMousePos().x, inputManager->getMousePos().y, -10);
				}

			}
		}
		/*
		list<GameObject *>::iterator iter;
		for (iter = gameObjects.begin(); iter != gameObjects.end(); iter++)
			(*iter)->update(time);
		*/
	}
	#pragma endregion
		
	cursor->setPosition(
					inputManager->getMousePos().x + camera.getPosition().x*100, 
					inputManager->getMousePos().y + camera.getPosition().y*100, 
					-1);

	return 0;
}

int Game::renderFrame(long time)
{
	#pragma region Pre Draw Variable Updates
	camera.setOrth(true);
	camera.setDimensions(wndWidth*0.01f, wndHeight*0.01f);
	camera.getViewMatrix(&viewMatrix);
	camera.getProjMatrix(&projMatrix);
	d3dDev->SetTransform(D3DTS_VIEW, &viewMatrix);
	d3dDev->SetTransform(D3DTS_PROJECTION, &projMatrix); 
	GameObject::setMatrices(&viewMatrix, &projMatrix);
	Quad::setMatrices(&viewMatrix, &projMatrix);
	#pragma endregion


	#pragma region Render to Screen Texture
	d3dDev->SetRenderTarget(0, renderSurface);
	//d3dDev->SetDepthStencilSurface(screenStencil);
	d3dDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(105, 143, 161), 1.0f, 0);
	d3dDev->BeginScene();

	list<GameObject *>::iterator iter;
	for (iter = gameObjects.begin(); iter != gameObjects.end(); iter++)
		(*iter)->renderFrame(time); 

	cursor->renderFrame(time);

	/*
	d3dDev->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	//draw debug wireframe stuff
	d3dDev->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	*/

	d3dDev->EndScene();
	#pragma endregion

	#pragma region Render to Backbuffer
	d3dDev->SetRenderTarget(0, backBuffer);
	camera.setDimensions(wndWidth, wndHeight);
	camera.getProjMatrix(&projMatrix);
	//d3dDev->SetTransform(D3DTS_PROJECTION, &projMatrix); 
	GameObject::setMatrices(&viewMatrix, &projMatrix);
	Quad::setMatrices(&viewMatrix, &projMatrix);
	
	//d3dDev->SetDepthStencilSurface(defStencil);
	d3dDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(200, 30, 150), 1.0f, 0);
	d3dDev->BeginScene();

	#pragma region Set Screen Texture Pos

	VERTEX* vertices;
	screenVertexBuffer->Lock(0, 0, (void**)&vertices, NULL);
	
	float width = surfaceDesc.Width;
	float height = surfaceDesc.Height;
	
	vertices[0].x = 0.5;//+0.5;
	vertices[0].y = 0.5;//+0.5;
	vertices[0].z = 0.0f;
	vertices[0].u = 1.0f;
	vertices[0].v = 1.0f;
	
	vertices[1].x = width+0.5;
	vertices[1].y = +0.5;
	vertices[1].z = 0.0f;
	vertices[1].u = 0.0f;
	vertices[1].v = 1.0f;
	
	vertices[2].x = +0.5;
	vertices[2].y = height+0.5;
	vertices[2].z = 0.0f;
	vertices[2].u = 1.0f;
	vertices[2].v = 0.0f;
		
	vertices[3].x = width+0.5;//width+0.5;
	vertices[3].y = height+0.5;//height+0.5;
	vertices[3].z = 0.0f;
	vertices[3].u = 0.0f;
	vertices[3].v = 0.0f;
	
	 
	screenVertexBuffer->Unlock();
	#pragma endregion

	/*
	D3DXVECTOR3 left = D3DXVECTOR3(-1,0,0);
	D3DXVECTOR3 up = D3DXVECTOR3(0,1,0);
	D3DXVECTOR3 forward = D3DXVECTOR3(0,0,1);
	D3DXVECTOR3 position = D3DXVECTOR3(surfaceDesc.Width/2, -surfaceDesc.Height/2.0f, 0);

	float y = surfaceDesc.Height/2;
	D3DXMATRIX screenMatrix(left.x,		left.y,		left.z,		0.0f,
							up.x,		up.y,		up.z,		0.0f,
							forward.x,	forward.y,	forward.z,	0.0f,
							position.x,	-y,	position.z,	1.0f);
	*/

	D3DXVECTOR3 position = D3DXVECTOR3(surfaceDesc.Width/2, surfaceDesc.Height/2, 0);

	D3DXMATRIX screenMatrix(	-1,			0,			0,	0.0f,
								0,			1,			0,	0.0f,
								0,			0,			1,	0.0f,
								position.x,	-position.y,0,	1.0f);

	screenMatrix *= viewMatrix * projMatrix;

	d3dDev->SetFVF(D3DFVF_VERTEX);
	d3dDev->SetStreamSource(0, screenVertexBuffer, 0, sizeof(VERTEX));

	if (screenEffect)
	{
		screenEffect->SetMatrix(hMatrix, &screenMatrix);
		screenEffect->SetTexture(hTexture, screenTexture);
		screenEffect->SetFloat(hTime, time);
		unsigned int numPasses = 1;
		screenEffect->Begin(&numPasses, 0);
		for (int i = 0; i < numPasses; i++) 
		{
			screenEffect->BeginPass(i);
			d3dDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
			screenEffect->EndPass();
		}
		screenEffect->End();
	}
	else 
	{
		loadScreenEffect();
		d3dDev->SetTexture(0, screenTexture);
		d3dDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}
	#pragma endregion

	#pragma region Render In Game UI
	//camera.setDimensions(wndWidth*0.01f, wndHeight*0.01f);
	//camera.getProjMatrix(&projMatrix);
	list<MenuWindow *>::iterator iter2;
	for (iter2 = menuObjects.begin(); iter2 != menuObjects.end(); iter2++)
		(*iter2)->renderFrame(time); 
	
	if (tab && gameState == IN_GAME)
	{
		list<MenuWindow *>::iterator iter3;
		iter3 = tabObjects.begin();
		(*iter3)->renderFrame(time);  iter3++;
		(*iter3)->renderFrame(time);  iter3++;
		for (iter3; iter3 != tabObjects.end(); iter3++)
		{
			if ((*iter3)->getPosition().y > -300 && (*iter3)->getPosition().y < 300)
			(*iter3)->renderFrame(time); 
		}
	}

	//dynamicsWorld->debugDrawWorld();
	//debugDrawer->drawLineBuffers();
	#pragma endregion

	#pragma region Draw Debug Text
	RECT tr;
	tr.top = 25;
	tr.left = sideWidth + 30 + 25;
	tr.right = sideWidth + 500;
	tr.bottom = 550;
	
	string s = "FPS: ";
	s += std::to_string(fps);

	font->DrawTextA(NULL, s.c_str(), -1, &tr, DT_TOP | DT_LEFT, D3DCOLOR_ARGB(255,255,0,0));
	#pragma endregion

	d3dDev->EndScene();
	d3dDev->Present(NULL, NULL, NULL, NULL);

	return 0;
}

int Game::cleanUp()
{	
	backBuffer->Release();
	renderSurface->Release();
	delete md3dpp;

	list<GameObject *>::iterator iter;
	for (iter = gameObjects.begin(); iter != gameObjects.end(); iter++)
	{
		if (*iter)
		{
			delete *iter;
			*iter = 0;
		}
	}
	gameObjects.clear();

	list<MenuWindow *>::iterator iter2;
	for (iter2 = menuObjects.begin(); iter2 != menuObjects.end(); iter2++)
	if (*iter2)
	{
		delete *iter2;
		*iter2 = 0;
	}
	menuObjects.clear();

	delete assetManager;
	while (gameObjects.size() > 0) gameObjects.pop_back();
	
	font->Release();
	d3dDev->Release();
	md3d->Release();

	FreeConsole();

	return 0;
}

void Game::changeState(int targetState)
{
	if (gameState == targetState) return;

	#pragma region Pre Change Cleanup

	if (cursor)
	{
		delete cursor;
		cursor = 0;
	}

	list<GameObject *>::iterator iter;
	for (iter = gameObjects.begin(); iter != gameObjects.end(); iter++)
	{
		if (*iter)
		{
			delete *iter;
			*iter = 0;
		}
	}
	gameObjects.clear();

	list<MenuWindow *>::iterator iter2;
	for (iter2 = menuObjects.begin(); iter2 != menuObjects.end(); iter2++)
	if (*iter2)
	{
		delete *iter2;
		*iter2 = 0;
	}
	menuObjects.clear();

	list<MenuWindow *>::iterator iter3;
	for (iter3 = tabObjects.begin(); iter3 != tabObjects.end(); iter3++)
	if (*iter3)
	{
		delete *iter3;
		*iter3 = 0;
	}
	tabObjects.clear();
	
	camera.setPosition(0,0,0);

	#pragma endregion

	if (targetState == START_MENU)
	{
		//ShowCursor(true);

		MenuWindow * title = new MenuWindow("Assets\\pixeltest.png");
		title->setPosition(0,0,MID);
		title->setSize(wndWidth, wndHeight);
		menuObjects.push_front(title);
	}
	else if (targetState == IN_GAME)
	{
		tab = false;
		pageNumber = 0;
		camera.setPosition(0,0,0);

		float sidePad = 0;
		int numSideBar = 0;

		int x = 1;
		while (x < (wndHeight-1-sideWidth))
			{ numSideBar++; x += sideWidth + sidePad; }

		float excessPad = (int)wndHeight % sideWidth;
		sidePad = excessPad / (float)(numSideBar-1.0f);

		//excessPad = wndHeight - numSideBar*sideWidth - (numSideBar-1)*sidePad;
		//excessPad /= 2.0;

		selectedRect = new GameObject("Assets\\UI\\red.png");
		selectedRect->setSize(5,5);
		gameObjects.push_back(selectedRect);

		MenuWindow * sideBarBack = new MenuWindow("Assets\\UI\\black.png");
		sideBarBack->setSize(sideWidth+31,wndHeight);
		sideBarBack->setPosition(0, 0, MID_LEFT);
		menuObjects.push_back(sideBarBack);

		MenuWindow * sideBarFront = new MenuWindow("Assets\\UI\\dark blue.png");
		sideBarFront->setSize(sideWidth+30,wndHeight);
		sideBarFront->setPosition(0, 0, MID_LEFT);
		menuObjects.push_back(sideBarFront);

		MenuWindow * tabBack = new MenuWindow("Assets\\UI\\black.png");
		tabBack->setSize(1040, 632);
		tabBack->setPosition(0,0, MID);
		tabObjects.push_back(tabBack);

		MenuWindow * tabWindow = new MenuWindow("Assets\\UI\\dark blue.png");
		tabWindow->setSize(1038, 630);
		tabWindow->setPosition(0,0, MID);
		tabObjects.push_back(tabWindow);

		for (int x = 0; x < 10; x++)
		{
			for (int y = 0; y < numTextures/10+1; y++)
			{
				if (x+y*10 < numTextures)
				{
					MenuWindow * tabBox = new MenuWindow(textureStrings[x][y].c_str());
					tabBox->setSize(100,100);
					tabBox->setPosition(-509+x*100+x*2+50, 306-y*100-y*2-51,MID);
					tabObjects.push_back(tabBox);
				}
			}
		}

		selectedSideBar = new MenuWindow("Assets\\UI\\light grey.png");
		selectedSideBar->setSize(30,sideWidth);
		selectedSideBar->setPosition(-100, sideWidth/2+1);
		menuObjects.push_back(selectedSideBar);

		x = sidePad/2;
		int index = 1;
		while (x < (wndHeight-sideWidth))
		{
			if (index < 11)
			{
				sideBar[index-1] = new MenuWindow("Assets\\UI\\dark grey.png");
				sideBar[index-1]->setSize(sideWidth, sideWidth);
				sideBar[index-1]->setPosition(sidePad/2, -x, TOP_LEFT);
				menuObjects.push_back(sideBar[index-1]);

				string indexPath;
				if (index == 10) indexPath = "Assets\\UI\\num0.png";
				else indexPath = "Assets\\UI\\num" + std::to_string(index) + ".png";
				MenuWindow * sideIconNum = new MenuWindow(indexPath.c_str());
				sideIconNum->setSize(30, 30);
				sideIconNum->setPosition(sidePad/2 + sideWidth, -x-15, TOP_LEFT);
				menuObjects.push_back(sideIconNum);
			}
			x += sideWidth + sidePad;
			index++;
		}

		//crosshair = new MenuWindow("Assets\\Player\\Crosshair.png");
		//crosshair->setSize(50,50);
		//menuObjects.push_back(crosshair);
	}
	cursor = new GameObject("Assets\\UI\\quill.png");
	cursor->setSize(100,100);
	//gameObjects.push_back(cursor);

	gameState = targetState;
}

void Game::changePage(short x)
{
	x = -x/abs(x);
	if (x == 0) return;
	//if (abs(x) < 100) return;
	
	if ((pageNumber + x) < 0) return;
	if ((pageNumber + x) > numTextures/60) return;

	list<MenuWindow *>::iterator iter3;
	iter3 = tabObjects.begin();
	iter3++; iter3++;
	for (iter3; iter3 != tabObjects.end(); iter3++)
	{
		(*iter3)->modPosition(0,6*102*x);
	}
	pageNumber+=x;
}

POINT Game::getSelectedTabBar()
{
	POINT p;
	//1018x610
	if (inputManager->getMousePos().x < -509 || inputManager->getMousePos().x > 509 ||
		inputManager->getMousePos().y < -306 || inputManager->getMousePos().y > 306)
	{
		p.x = -1;
		p.y = -1;
	}
	else
	{
		//-509+x*100+x*2+50, 306-y*100-y*2-51
		p.x = (inputManager->getMousePos().x + 509) / 102;
		p.y = (-inputManager->getMousePos().y + 306) / 102;
	}
	return p;
}

int Game::getSelectedSideBar()
{
	int i = 0;
	float sidePad = 0;
	int numSideBar = 0;

	int x = 1;
	while (x < (wndHeight-1-sideWidth))
		{ numSideBar++; x += sideWidth + sidePad; }

	float excessPad = (int)wndHeight % sideWidth;
	sidePad = excessPad / (float)(numSideBar-1.0f);

	float y = -inputManager->getMousePos().y + wndHeight/2;
	float yd = sidePad/2;
	int yi = 0;
	while (yd < wndHeight)
	{
		if (yd < y && yd+sideWidth > y) return yi;
		yd += sideWidth + sidePad;
		yi++;
	}

	return -1;
}

int Game::loadScreenEffect(void)
{
	assetManager->getEffect("FullScreen.hlsl", &screenEffect);

	if (screenEffect)
	{
		hTexture = screenEffect->GetParameterByName(NULL, "tex");
		hTime = screenEffect->GetParameterByName(NULL, "time");
		hMatrix = screenEffect->GetParameterByName(NULL, "worldViewProj");
		hTechnique = screenEffect->GetTechniqueByName("FullScreenTechnique");
		screenEffect->SetTechnique(hTechnique);
		return 1;
	}

	return 0;
}

int Game::resizeWindow(int width, int height)
{
	wndWidth = width;
	wndHeight = height;

	md3dpp->BackBufferWidth = wndWidth;
	md3dpp->BackBufferHeight = wndHeight;

	//d3dDev->Reset(md3dpp);

	return 0;
}

#pragma region Framerate Functions
void Game::startTimer()
{
	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);

	countsPerSecond = double(frequencyCount.QuadPart);

	QueryPerformanceCounter(&frequencyCount);
	CounterStart = frequencyCount.QuadPart;
}

double Game::getTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart-CounterStart)/countsPerSecond;
}

double Game::getFrameTime()
{
	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart-frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if(tickCount < 0.0f)
		tickCount = 0.0f;

	return float(tickCount)/countsPerSecond;
}
#pragma endregion

#pragma region Thread Functions
unsigned int WINAPI Game::startLoader(LPVOID)
{
	cout << "Starting Asset Loader\n";

	AssetLoader assetLoader;
	assetLoader.ptextures = assetManager->getTextureAddress();
	assetLoader.peffects = assetManager->getEffectAddress();
	assetLoader.pobjectsToLoad = assetManager->getObjectToLoadAddress();

	assetLoader.start();

	return 0;
}
#pragma endregion