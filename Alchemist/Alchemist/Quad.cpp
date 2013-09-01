#include "Quad.h"

IDirect3DDevice9 * Quad::d3dDev = NULL;
AssetManager * Quad::assetManager = NULL;
D3DXMATRIX *Quad::viewMatrix = NULL;
D3DXMATRIX *Quad::projMatrix = NULL;
const D3DVERTEXELEMENT9 VERTEX::Decl[] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
    { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    D3DDECL_END()
};

Quad::Quad() : position(0,0,0), lookAt(0,0,-1), upVector(0,1,0), width(640.0f), height(360.0f), texture(0), vertexBuffer(0)
{
	directory = "";
}

Quad::~Quad()
{}

int Quad::init(const char *dir)
{
	directory = dir;
	int x = 2;

	//Set Dimensions
	D3DXIMAGE_INFO SrcInfo;
	D3DCOLOR colorkey = 0xFFFF00FF;

	//Load Texture
	if (texture == 0) assetManager->getTexture(dir, &texture);

	if (vertexBuffer == 0) 
		d3dDev->CreateVertexBuffer(sizeof(VERTEX) * 4, NULL, D3DFVF_VERTEX, D3DPOOL_MANAGED, &vertexBuffer, NULL);

	return 0;
}

int Quad::setD3dDev(IDirect3DDevice9 *Dev)
{
	d3dDev = Dev;
	return 0;
}

void Quad::setViewMatrix(D3DXMATRIX * matrix) { viewMatrix = matrix; }
void Quad::setProjMatrix(D3DXMATRIX * matrix) { projMatrix = matrix; }
void Quad::setMatrices(D3DXMATRIX * view, D3DXMATRIX * proj)
{
	setViewMatrix(view);
	setProjMatrix(proj);
}

int Quad::setAssetManager(AssetManager *ass)
{
	assetManager = ass;
	return 0;
}

int Quad::update(long time)
{
	modSize(0.5f,0.5f);
	return 0;
}

/*
0--1
|\ |
| \|
3--2
*/

int Quad::render(long time)
{
	if (texture == 0 || vertexBuffer == 0) 
		init(directory.c_str());
	else
	{
		VERTEX* vertices;
		vertexBuffer->Lock(0, 0, (void**)&vertices, NULL);
	
		float hWidth = width/2.0f;
		float hHeight = height/2.0f;
		
		vertices[0].x = -0.5f;
		vertices[0].y = -0.5f;
	    vertices[0].z = 0.0f;
		vertices[0].u = 1.0f;
	    vertices[0].v = 1.0f;
	
	    vertices[1].x = 0.5f;
	    vertices[1].y = -0.5f;
	    vertices[1].z = 0.0f;
		vertices[1].u = 0.0f;
	    vertices[1].v = 1.0f;
	
		vertices[2].x = -0.5f;
	    vertices[2].y = 0.5f;
	    vertices[2].z = 0.0f;
		vertices[2].u = 1.0f;
	    vertices[2].v = 0.0f;
		
		vertices[3].x = 0.5f;
	    vertices[3].y = 0.5f;
	    vertices[3].z = 0.0f;
		vertices[3].u = 0.0f;
	    vertices[3].v = 0.0f;
		

		/*
		vertices[0].x = -hWidth		- 0.5f;
		vertices[0].y = -hHeight	- 0.5f;
	    vertices[0].z = 0.0f;
		vertices[0].u = 1.0f;
	    vertices[0].v = 1.0f;
	
	    vertices[1].x = hWidth		- 0.5f;
	    vertices[1].y = -hHeight	- 0.5f;
	    vertices[1].z = 0.0f;
		vertices[1].u = 0.0f;
	    vertices[1].v = 1.0f;
	
		vertices[2].x = -hWidth		- 0.5f;
	    vertices[2].y = hHeight		- 0.5f;
	    vertices[2].z = 0.0f;
		vertices[2].u = 1.0f;
	    vertices[2].v = 0.0f;
		
		vertices[3].x = hWidth		- 0.5f;
	    vertices[3].y = hHeight		- 0.5f;
	    vertices[3].z = 0.0f;
		vertices[3].u = 0.0f;
	    vertices[3].v = 0.0f;
		*/

		vertexBuffer->Unlock();
	
		d3dDev->SetFVF(D3DFVF_VERTEX);
		d3dDev->SetStreamSource(0, vertexBuffer, 0, sizeof(VERTEX));
		d3dDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}	

	return 0;
}

void Quad::modSize(float X, float Y) { width += X; height += Y; }
void Quad::setSize(float Width, float Height) { width = Width; height = Height; }
void Quad::modPos(float X, float Y, float Z) { position.x += X; position.y += Y; position.z += Z; }
void Quad::modPos(D3DXVECTOR3 Mod) { position += Mod; }
void Quad::setPos(float X, float Y, float Z) { position = D3DXVECTOR3(X, Y, Z); }
void Quad::setPos(D3DXVECTOR3 Pos) { position = Pos; }