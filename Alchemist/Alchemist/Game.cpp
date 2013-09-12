#include "Game.h"

#pragma region Game Constants
#define WND_WIDTH 1280
#define WND_HEIGHT 800
#define FRAMES_PER_SECOND 60

#define START_MENU	0
#define PAUSE_MENU	1
#define IN_GAME		2
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
												menuObjects(), gameObjects(), physObjects(), player(0), sunDir(1,-1,0), screenEffect(0),
												renderSurface(NULL), backBuffer(NULL)
{ 
	fullScreen = false;
	console = false;
	networkState = ONLINE_STATE;

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

	/*
	POINT windowPos;
	D3DDEVICE_CREATION_PARAMETERS cparams;
	RECT winRect;

	d3dDev->GetCreationParameters(&cparams);
	GetWindowRect(cparams.hFocusWindow, &winRect);

	windowPos.x = winRect.right/2 - wndWidth/2;
	windowPos.y = winRect.bottom/2 - wndHeight/2;
	*/

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

	//d3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );
    //d3dDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    d3dDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW  );
    d3dDev->SetRenderState( D3DRS_ZENABLE, TRUE );
	d3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );

	//d3dDev->SetRenderState( D3DRS_ZENABLE,  TRUE );
    //d3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );
    //d3dDev->SetRenderState( D3DRS_FOGCOLOR, BACKGROUND_COLOR );
    //d3dDev->SetRenderState( D3DRS_FOGSTART, *((DWORD*)&(fConv = 0.0f)) );
    //d3dDev->SetRenderState( D3DRS_FOGEND,   *((DWORD*)&(fConv = 100.0f)) );
    //d3dDev->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
    //d3dDev->SetRenderState( D3DRS_FOGENABLE,    TRUE );
    //d3dDev->SetRenderState( D3DRS_DITHERENABLE, TRUE );

	//Alpha Blending
	d3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	d3dDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	d3dDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	d3dDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	d3dDev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	d3dDev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);

	//d3dDev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
    //d3dDev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    //d3dDev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

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

	#pragma region Initialize Bullet Physics
	///collision configuration contains default setup for memory, collision setup
	collisionConfiguration = new btDefaultCollisionConfiguration();
	//m_collisionConfiguration->setConvexConvexMultipointIterations();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	dispatcher = new	btCollisionDispatcher(collisionConfiguration);

	btVoronoiSimplexSolver* simplex = new btVoronoiSimplexSolver();
	btMinkowskiPenetrationDepthSolver* pdSolver = new btMinkowskiPenetrationDepthSolver();


	btConvex2dConvex2dAlgorithm::CreateFunc* convexAlgo2d = new btConvex2dConvex2dAlgorithm::CreateFunc(simplex,pdSolver);
	
	dispatcher->registerCollisionCreateFunc(CONVEX_2D_SHAPE_PROXYTYPE,CONVEX_2D_SHAPE_PROXYTYPE,convexAlgo2d);
	dispatcher->registerCollisionCreateFunc(BOX_2D_SHAPE_PROXYTYPE,CONVEX_2D_SHAPE_PROXYTYPE,convexAlgo2d);
	dispatcher->registerCollisionCreateFunc(CONVEX_2D_SHAPE_PROXYTYPE,BOX_2D_SHAPE_PROXYTYPE,convexAlgo2d);
	dispatcher->registerCollisionCreateFunc(BOX_2D_SHAPE_PROXYTYPE,BOX_2D_SHAPE_PROXYTYPE,new btBox2dBox2dCollisionAlgorithm::CreateFunc());

	broadphase = new btDbvtBroadphase();
	//m_broadphase = new btSimpleBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
	solver = sol;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	//m_dynamicsWorld->getSolverInfo().m_erp = 1.f;
	//m_dynamicsWorld->getSolverInfo().m_numIterations = 4;
	
	dynamicsWorld->setGravity(btVector3(0,-10,0));

	debugDrawer = new BulletDebugDrawer(d3dDev);
	dynamicsWorld->setDebugDrawer(debugDrawer);
	dynamicsWorld->getDebugDrawer()->setDebugMode(0);

	initGround();

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
	RigidObject::setStatics(&collisionShapes, dynamicsWorld, &physObjects);
	Player::setInputManager(inputManager);
	Character::setProjectileManager(&projectileManager);
	Model::setd3dDev(d3dDev);
	SkinnedData::setd3dDev(d3dDev);
	MenuWindow::setStatics(&camera, wndWidth, wndHeight);
	Packet::setList(&gameObjects);
	std::cout << "Static References Set\n";
	#pragma endregion

	loadScreenEffect();

	_beginthreadex(NULL, 0, startLoader, NULL, NULL, NULL);		//Asset Loader
	_beginthreadex(NULL, 0, startListener, NULL, NULL, NULL);	//Network Listener

	return 0;
}

int Game::initGround()
{
	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(5.),btScalar(50.),btScalar(150.)));
//	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),50);
	
	collisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,-50,0));

	//We can also use DemoApplication::localCreateRigidBody, but for clarity it is provided here:
	btScalar mass(0.);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		groundShape->calculateLocalInertia(mass,localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	//add the body to the dynamics world
	dynamicsWorld->addRigidBody(body);
	
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

		#pragma region Update Bullet
		dynamicsWorld->stepSimulation(1.f/60.f,1);
		dynamicsWorld->clearForces();

		//Assume world->stepSimulation or world->performDiscreteCollisionDetection has been called
 
		int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
		for (int i=0;i<numManifolds;i++)
		{
			btPersistentManifold* contactManifold =  dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
			const btCollisionObject* obA = static_cast<const btCollisionObject*>(contactManifold->getBody0());
			const btCollisionObject* obB = static_cast<const btCollisionObject*>(contactManifold->getBody1());
 
			int numContacts = contactManifold->getNumContacts();
			for (int j=0;j<numContacts;j++)
			{
				btManifoldPoint& pt = contactManifold->getContactPoint(j);
				if (pt.getDistance()<0.f)
				{
					RigidObject * physA = getRigidObject(obA);
					RigidObject * physB = getRigidObject(obB);

					physA->collide(physB, &pt.getPositionWorldOnB());
					physB->collide(physA, &pt.getPositionWorldOnA());
				}
			}
		}
		#pragma endregion
		
		if (inputManager->keyPress(VK_ESCAPE)) 
		{
			changeState(START_MENU);
			return 0;
		}
		
		if (inputManager->keyPress('O')) 
		{
			changeState(START_MENU);
			changeState(IN_GAME);
			return 0;
		}

		//====================================================
		//================= FIX PACKET FIRST =================
		//====================================================
		//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
		/*	
		if (inputManager->getKey('P') == 1) 
		{
			if (networkState == ONLINE_STATE)
			{
				networkState = CLIENT_WAITING;

				gameObjects.clear();
				char * ip = new char[16];
				strcpy(ip, "192.168.11.109");
			
				_beginthreadex(NULL, 0, clientThread, (void*)ip, NULL, NULL);
			}
		}

		*/
		//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
		//====================================================
		//================= FIX PACKET FIRST =================
		//====================================================

		D3DXVECTOR2 p = inputManager->getMousePos();

		string px = std::to_string((int)p.x);
		string py = std::to_string((int)p.y);
		if (p.x < 0) px = px.substr(1, px.length());
		if (p.y < 0) py = py.substr(1, py.length());
		if (px.length() < 3) px = "0" + px;
		if (px.length() < 3) px = "0" + px;
		if (py.length() < 3) py = "0" + py;
		if (py.length() < 3) py = "0" + py;
		px = px.insert(px.length()-2, ".");
		py = py.insert(py.length()-2, ".");
		if (p.x < 0) px = px.insert(0, "-");
		if (p.y < 0) py = py.insert(0, "-");

		double dx = stod(px);
		double dy = stod(py);

		player->setCursorPos(dx, dy);
		//crosshair->setPosition(cp.x, cp.y);

		list<GameObject *>::iterator iter;
		for (iter = gameObjects.begin(); iter != gameObjects.end(); iter++)
			(*iter)->update(time);

		//player->setPosition(0, 10, 0);
		//camera.setPosition(player->getPosition());	
	}
	#pragma endregion
		
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
	d3dDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(150, 150, 100), 1.0f, 0);
	d3dDev->BeginScene();

	list<GameObject *>::iterator iter;
	for (iter = gameObjects.begin(); iter != gameObjects.end(); iter++)
		(*iter)->renderFrame(time); 

	projectileManager.render(time);
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
	
	//dynamicsWorld->debugDrawWorld();
	//debugDrawer->drawLineBuffers();
	#pragma endregion

	#pragma region Draw Debug Text
	RECT tr;
	tr.top = 70;
	tr.left = 50;
	tr.right = 500;
	tr.bottom = 500;
	
	string s = "FPS: ";
	s += std::to_string(fps);
	s += "\nActive: ";
	s += activeWindow ? "true" : "false";
	s += "\nNumber of Bullets: " + std::to_string(projectileManager.getSize());

	font->DrawTextA(NULL, s.c_str(), -1, &tr, DT_TOP | DT_LEFT, D3DCOLOR_ARGB(255,255,0,0));
	#pragma endregion

	d3dDev->EndScene();
	d3dDev->Present(NULL, NULL, NULL, NULL);

	return 0;
}

int Game::cleanUp()
{	
	#pragma region Bullet
	cleanBullet();

	delete dynamicsWorld;
	
	delete solver;
	
	delete broadphase;
	
	delete dispatcher;

	delete collisionConfiguration;

	//delete debugDrawer;

	dynamicsWorld = 0;
	solver = 0;
	broadphase = 0;
	dispatcher = 0;
	collisionConfiguration = 0;
	#pragma endregion

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

int Game::cleanBullet()
{
	int i;
	if (dynamicsWorld)
	for (i=dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}
	
	//delete collision shapes
	for (int j=0;j<collisionShapes.size();j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		delete shape;
	}

	collisionShapes.clear();
	
	return 0;
}

void Game::changeState(int targetState)
{
	if (gameState == targetState) return;

	#pragma region Pre Change Cleanup

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

	projectileManager.clearList();

	cleanBullet();

	camera.setPosition(0,0,0);

	#pragma endregion

	if (targetState == START_MENU)
	{
		//ShowCursor(true);

		MenuWindow * title = new MenuWindow("Assets\\reds.png");
		title->setPosition(0,0,MID);
		title->setSize(wndWidth, wndHeight);
		menuObjects.push_front(title);
	}
	else if (targetState == IN_GAME)
	{
		ShowCursor(false);
		camera.setPosition(0,wndHeight/4,0);
		//initGround();
		
		player = new Player();
		player->setPosition(0,650,0);
		player->setSize(100, 100);
		GameObject * p = player;
		gameObjects.push_front(p);

		float x;
		float y;
		float maxDist = 300;

		for (int i = 0; i < 25; i++)
		{
			RigidObject * temp = new RigidObject("Assets\\crate.png");

			x = rand() % 100;
			y = rand() % 100;
			x *= 0.01;
			y *= 0.01;
			temp->setPosition(x*maxDist*2 - maxDist, 500+y*maxDist*2 - maxDist, 0);
			
			x = rand() % 100;
			y = rand() % 100;
			if (x < 50) x += 50;
			//x -= 25;
			temp->setSize(x,x);
			temp->setMass(x*x*0.001);

			GameObject * tempObj = temp;
			gameObjects.push_front(tempObj);
		}

		RigidObject * ground = new RigidObject("Assets\\terr.png");
		ground->setMass(0);
		ground->setSize(1000,300);
		ground->setPosition(0, -150, 0);
		GameObject * groundObj = ground;
		gameObjects.push_front(groundObj);
		
		MenuWindow * healthBars = new MenuWindow("Assets\\HealthTemp.png");
		healthBars->setSize(300,75);
		healthBars->setPosition(50, 50, BOT_LEFT);
		menuObjects.push_front(healthBars);

		//crosshair = new MenuWindow("Assets\\Player\\Crosshair.png");
		//crosshair->setSize(50,50);
		//menuObjects.push_back(crosshair);
	}

	gameState = targetState;
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

RigidObject * Game::getRigidObject(const btCollisionObject * b)
{
	list<RigidObject *>::iterator iter;
	for (iter = physObjects.begin(); iter != physObjects.end(); iter++)
	{
		if ((*iter)->getBody() == b) 
			return (*iter);
	}
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
	assetLoader.pmodels = assetManager->getModelAddress();
	assetLoader.ptextures = assetManager->getTextureAddress();
	assetLoader.peffects = assetManager->getEffectAddress();
	assetLoader.pobjectsToLoad = assetManager->getObjectToLoadAddress();

	assetLoader.start();

	return 0;
}

#define DEFAULT_PORT			5150
#define DEFAULT_BUFFER			4096

unsigned int WINAPI Game::startListener(LPVOID)
{
	int		iPort		= DEFAULT_PORT; // Port to listen for clients on
	BOOL	bInterface	= FALSE,	 // Listen on the specified interface
			bRecvOnly	= FALSE;   // Receive data only; don't echo back
	char	szAddress[128];       // Interface to listen for clients on

	WSADATA				wsd;
    SOCKET				sListen,
						sClient;
    int					iAddrSize;
    HANDLE				hThread;
    unsigned int		dwThreadId;
    struct sockaddr_in	local,
						client;

    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
        printf("Failed to load Winsock!\n");
        return 1;
    }
    // Create our listening socket
    //
    sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sListen == SOCKET_ERROR)
    {
        printf("socket() failed: %d\n", WSAGetLastError());
        return 1;
    }
    // Select the local interface and bind to it
    //
    if (false)
    {
        local.sin_addr.s_addr = inet_addr(szAddress);
        if (local.sin_addr.s_addr == INADDR_NONE)
            int x = 2;//usage();
    }
    else
        local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_family = AF_INET;
    local.sin_port = htons(iPort);

    if (bind(sListen, (struct sockaddr *)&local,
            sizeof(local)) == SOCKET_ERROR)
    {
        printf("bind() failed: %d\n", WSAGetLastError());
        return 1;
    }
    listen(sListen, 8);
    //
    // In a continous loop, wait for incoming clients. Once one
    // is detected, create a thread and pass the handle off to it.
    //

	printf("Successfully launched Network Listener\n");

    while (1)
    {
        iAddrSize = sizeof(client);
        sClient = accept(sListen, (struct sockaddr *)&client,
                        &iAddrSize);
        if (sClient == INVALID_SOCKET)
        {
            printf("accept() failed: %d\n", WSAGetLastError());
            break;
        }

		numClients++;
		networkState = HOST_STATE;

        printf("Accepted client: %s:%d\n",
            inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        _beginthreadex(NULL, 0, hostThread,
                    (LPVOID)sClient, 0, &dwThreadId);
        /*if (hThread == NULL)
        {
            printf("CreateThread() failed: %d\n", GetLastError());
            break;
        }*/
        //CloseHandle(hThread);
    }
    closesocket(sListen);

    WSACleanup();
    return 0;
}

unsigned int WINAPI Game::hostThread(LPVOID lpParam)
{
	SOCKET        sock=(SOCKET)lpParam;
    char          szBuff[DEFAULT_BUFFER];
    int           ret,
                  nLeft,
                  idx;

    while(1)
    {
        // Perform a blocking recv() call
        //
        ret = recv(sock, szBuff, DEFAULT_BUFFER, 0);
        if (ret == 0)        // Graceful close
            break;
        else if (ret == SOCKET_ERROR)
        {
            printf("recv() failed: %d\n", WSAGetLastError());
            break;
        }

        szBuff[ret] = '\0';
        //printf("Host: RECV: '%s'\n", szBuff);

		char * gameObjectPacket = packet.serialize();
		memcpy(szBuff, gameObjectPacket, packet.getBufferSize());

        ret = send(sock, szBuff, packet.getBufferSize(), 0);
        if (ret == 0)
            break;
        else if (ret == SOCKET_ERROR)
        {
            printf("send() failed: %d\n",
                WSAGetLastError());
            break;
        }

		delete [] gameObjectPacket;
        
		Sleep(15);
    }
	printf("Connection to client lost. Closing thread.\n");
	numClients--;
	if (numClients == 0) networkState = ONLINE_STATE;	//no longer hosting, no more clients
	return 0;
}

#define DEFAULT_COUNT			20
#define DEFAULT_MESSAGE			"Test Test 123"

unsigned int WINAPI Game::clientThread(LPVOID lpParam)
{
	char	szServer[128],          // Server to connect to
			szMessage[DEFAULT_BUFFER];        // Message to send to sever
	int   iPort     = DEFAULT_PORT;  // Port on server to connect to
	DWORD dwCount   = DEFAULT_COUNT; // Number of times to send message
	BOOL  bSendOnly = FALSE;         // Send data only; don't receive

	WSADATA       wsd;
    SOCKET        sClient;
    char          szBuffer[DEFAULT_BUFFER];
    int           ret,
                  i;
    struct sockaddr_in server;
    struct hostent    *host = NULL;

	//iPort = (long) lpParam;

    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
        printf("Failed to load Winsock library!\n");
		networkState = ONLINE_STATE;
        return 1;
    }
    strcpy(szMessage, DEFAULT_MESSAGE);
    //
    // Create the socket, and attempt to connect to the server
    //
	char * ip = (char*)lpParam;
	printf("Attempting to connect to %s:%d%\n", ip, iPort);
	
    sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sClient == INVALID_SOCKET)
    {
        printf("socket() failed: %d\n", WSAGetLastError());
		networkState = ONLINE_STATE;
        return 1;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(iPort);
    server.sin_addr.s_addr = inet_addr(ip);
	
	delete[] ip;
	
    //
    // If the supplied server address wasn't in the form
    // "aaa.bbb.ccc.ddd" it's a hostname, so try to resolve it
    //
    if (server.sin_addr.s_addr == INADDR_NONE)
    {
        host = gethostbyname(szServer);
        if (host == NULL)
        {
            printf("Unable to resolve server: %s\n", szServer);
			networkState = ONLINE_STATE;
            return 1;
        }
        CopyMemory(&server.sin_addr, host->h_addr_list[0],
            host->h_length);
    }
    if (connect(sClient, (struct sockaddr *)&server,
        sizeof(server)) == SOCKET_ERROR)
    {
        printf("connect() failed: %d\n", WSAGetLastError());
		networkState = ONLINE_STATE;
        return 1;
    }
	char * serverIp = inet_ntoa(server.sin_addr);
	int serverPort = server.sin_port;
	printf("Connected to %s:%d\n", serverIp, serverPort);

	networkState = CLIENT_CONNECTED;

    // Send and receive data
    //
    while (true) //change to while(true) later
    {
		szBuffer[0] = '\0';
		strcat(szBuffer, "Client to Host");
        ret = send(sClient, szBuffer, strlen(szBuffer), 0);
        if (ret == 0)
            break;
        else if (ret == SOCKET_ERROR)
        {
            printf("send() failed: %d\n", WSAGetLastError());
            break;
        }
        //printf("Client: SENT: [%d bytes]: '%s'\n", ret, szBuffer);

        ret = recv(sClient, szBuffer, DEFAULT_BUFFER, 0);
        if (ret == 0)        // Graceful close
            break;
        else if (ret == SOCKET_ERROR)
        {
            printf("recv() failed: %d\n", WSAGetLastError());
            break;
        }
        //szBuffer[ret] = '\0';
        //printf("Client: RECV: [%d bytes]: '%s'\n", ret, szBuffer);
		packet.deserialize(szBuffer);

		Sleep(10);
    }
	networkState = ONLINE_STATE;
    closesocket(sClient);
	printf("Closing Socket\n");
    WSACleanup();
    return 0;
}
#pragma endregion