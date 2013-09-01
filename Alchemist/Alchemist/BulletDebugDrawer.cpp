#include "BulletDebugDrawer.h"

BulletDebugDrawer::BulletDebugDrawer(LPDIRECT3DDEVICE9 lpDevice): device(lpDevice), m_debugMode(0), numLines(0), numLinesVerts(0), numLinesVertsMax(1024)
{

}


BulletDebugDrawer::~BulletDebugDrawer()
{
}

void BulletDebugDrawer::drawLineBuffers()
{
	/*
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity( &matWorld );
	device->SetTransform( D3DTS_WORLD, &matWorld );
	//device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetVertexShader( NULL );
	device->SetPixelShader( NULL );
	device->SetFVF( D3DFVF_CUSTOMVERTEX_VC );
	if( numLines>0 )
		device->DrawPrimitiveUP( D3DPT_LINELIST, numLines, gLines, sizeof(CUSTOMVERTEX_VC) );
	numLinesVerts=0;	
	numLines=0;

	device->SetRenderState(D3DRS_ZENABLE, TRUE);
	*/
}

void BulletDebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& fromColor, const btVector3& toColor)
{

}

void BulletDebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
{
	//Too many verts for temp buffer
	if( numLinesVerts>=numLinesVertsMax )
	{
		//Flush the buffer and continue
		drawLineBuffers();
	}

	int colour = 0xff000000 | ((int)color[0]<<16) | ((int)color[1]<<8) | ((int)color[2]);

	gLines[numLinesVerts].x = from[0];
	gLines[numLinesVerts].y = from[1];
	gLines[numLinesVerts].z = from[2];
	gLines[numLinesVerts++].color = colour;

	gLines[numLinesVerts].x = to[0];
	gLines[numLinesVerts].y = to[1];
	gLines[numLinesVerts].z = to[2];
	gLines[numLinesVerts++].color = colour;

	numLines++;
}

/*
void BulletDebugDrawer::drawBox(const btVector3& bbMin, const btVector3& bbMax, const btVector3& color)
{


}


void BulletDebugDrawer::drawSphere (const btVector3& p, btScalar radius, const btVector3& color)
{

}

void BulletDebugDrawer::drawTriangle(const btVector3& a,const btVector3& b,const btVector3& c,const btVector3& color,btScalar alpha)
{

}
*/
void BulletDebugDrawer::setDebugMode(int debugMode)
{

}

void BulletDebugDrawer::draw3dText(const btVector3& location,const char* textString)
{

}

void BulletDebugDrawer::drawContactPoint(const btVector3& pointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
{

}

void BulletDebugDrawer::reportErrorWarning(const char* warningString)
{

}