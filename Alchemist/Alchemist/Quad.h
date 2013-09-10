#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include "AssetManager.h"

const DWORD D3DFVF_VERTEX = (D3DFVF_XYZ | D3DFVF_TEX1);

struct VERTEX
{
	float x, y, z;
	float u, v;

	const static D3DVERTEXELEMENT9 Decl[4];
};

class Quad
{
public:
	Quad();
	~Quad();

	static int setD3dDev(IDirect3DDevice9*);
	static int setAssetManager(AssetManager*);
	static void setViewMatrix(D3DXMATRIX*);
	static void setProjMatrix(D3DXMATRIX*);
	static void setMatrices(D3DXMATRIX*, D3DXMATRIX*);

	int init();
	/*void modSize(float X, float Y);
	void setSize(float Width, float Height);
	void modPos(float X, float Y, float Z);
	void modPos(D3DXVECTOR3 Mod);
	void setPos(float X, float Y, float Z);
	void setPos(D3DXVECTOR3 Pos);
	*/
	bool vertexLoaded();
	int render(long time);
	int update(long time);

private:
	static IDirect3DDevice9* d3dDev;
	static AssetManager *assetManager;

	static D3DXMATRIX *viewMatrix;
	static D3DXMATRIX *projMatrix;

	IDirect3DVertexBuffer9* vertexBuffer;
};