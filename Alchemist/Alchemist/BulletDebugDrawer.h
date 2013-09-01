#pragma once

#include <d3dx9.h>
#include "btBulletDynamicsCommon.h"

class BulletDebugDrawer : public btIDebugDraw
{
int m_debugMode;

public:
	BulletDebugDrawer(LPDIRECT3DDEVICE9 device);
	virtual ~BulletDebugDrawer();

	virtual void drawLineBuffers();
	virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& fromColor, const btVector3& toColor);
	virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color);

	virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);

	virtual void reportErrorWarning(const char* warningString);

	virtual void draw3dText(const btVector3& location,const char* textString);

	virtual void setDebugMode(int debugMode);
	virtual int getDebugMode() const { return m_debugMode;}

private:
	struct CUSTOMVERTEX_VC
	{
		FLOAT x, y, z;	 //Position
		DWORD color;	 //Colour
	};
	#define D3DFVF_CUSTOMVERTEX_VC (D3DFVF_XYZ|D3DFVF_DIFFUSE)	

	//Globals
	const int numLinesVertsMax;// = 1024;
	CUSTOMVERTEX_VC gLines[1024];
	int numLinesVerts;
	int numLines;

	IDirect3DDevice9 * device;
};