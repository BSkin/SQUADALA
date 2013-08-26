#include "MenuWindow.h"

int MenuWindow::wndWidth = 0;
int MenuWindow::wndHeight = 0;
Camera * MenuWindow::camera = 0;

MenuWindow::MenuWindow() : width(wndWidth), height(wndHeight), cameraOffset(0.0f,0.0f,0.0f), screenX(0.0f), screenY(0.0f)
{}

MenuWindow::MenuWindow(const char*dir) : width(wndWidth), height(wndHeight), cameraOffset(0.0f,0.0f,0.0f), screenX(0.0f), screenY(0.0f)
{ init(dir); }

MenuWindow::~MenuWindow() 
{}

int MenuWindow::init(const char *dir) 
{ 
	//updateOffset();  
	return quad.init(dir); 
}

void MenuWindow::setSize(int x, int y)
{
	width = x;
	height = y;

	quad.setSize(width, height);
}

void MenuWindow::setSize(float x, float y)
{
	if (x > 1.0f) x = 1.0f;
	if (x < 0.0f) x = 0.0f;
	if (y > 1.0f) y = 1.0f;
	if (y < 0.0f) y = 0.0f;

	width = wndWidth * x;
	height = wndHeight * y;

	quad.setSize(width, height);
}

void MenuWindow::setPosition(int x, int y, int anchor)
{
	int x0 = 0;
	int y0 = 0;

	switch (anchor)
	{
		case TOP_LEFT : 
			x0 = width/2;
			y0 = height/2;
			break;
		case TOP_RIGHT :
			x0 = wndWidth - width/2;
			y0 = height/2;
			break;
		case BOT_LEFT :
			x0 = width/2;
			y0 = wndHeight - height/2;
			break;
		case BOT_RIGHT :
			x0 = wndWidth - width/2;
			y0 = wndHeight - height/2;
			break;
		case MID_TOP :
			x0 = wndWidth/2;
			y0 = height/2;
			break;
		case MID_BOT :
			x0 = wndWidth/2;
			y0 = wndHeight - height/2;
			break;
		case MID_LEFT :
			x0 = width/2;
			y0 = wndHeight/2;
			break;
		case MID_RIGHT :
			x0 = wndWidth - width/2;
			y0 = wndHeight/2;
			break;
		case MID :
			x0 = wndWidth/2;
			y0 = wndHeight/2;
			break;
		default :
			x0 = wndWidth/2;
			y0 = wndHeight/2;
			break;
	}

	x0 += x;
	y0 += y;

	screenX = x0;
	screenY = y0;
	updateOffset(); 
	quad.setPos(cameraOffset);
}

void MenuWindow::setPosition(float x, float y)
{
	int ix = x*wndWidth;
	int iy = y*wndHeight;

	screenX = ix;
	screenY = iy;
	updateOffset(); 
	quad.setPos(cameraOffset);
}

void MenuWindow::modPosition(int x, int y)
{
	
}

void MenuWindow::modPosition(float x, float y)
{
	screenX += x;
	screenY += y;
	updateOffset();
	quad.setPos(cameraOffset);
}

int MenuWindow::renderFrame()
{
	return quad.render(D3DXVECTOR3(0,0,1), D3DXVECTOR3(0,1,0));
}

void MenuWindow::updateOffset()
{
	D3DXVECTOR3 camUp = D3DXVECTOR3(0,1,0);
	D3DXVECTOR3 camForward = D3DXVECTOR3(0,0,1);
	D3DXVECTOR3 camRight = D3DXVECTOR3(-1,0,0);
	
	cameraOffset = camForward +
		(camRight * (wndWidth/2 - screenX)) +
		(camUp * (wndHeight/2 - screenY));
}

void MenuWindow::setStatics(Camera * cam, int x, int y)
{
	camera = cam;
	wndWidth = x;
	wndHeight = y;
}