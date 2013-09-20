#pragma once

#include <d3dx9.h>
#include "Utility.h"
#include "Camera.h"
#include "AssetManager.h"
#include "Quad.h"

/*
#include <string>
#include <list>

using std::string;
using std::list;
*/
class GameObject
{
public:
	GameObject(void);
	GameObject(const char *fileBase, D3DXVECTOR3 pos = D3DXVECTOR3(0,0,0));
	GameObject(short ID, D3DXVECTOR3 pos = D3DXVECTOR3(0,0,0));
	~GameObject(void);

	static int setd3dDev(IDirect3DDevice9 *);
	static int setAssetManager(AssetManager *);
	static int setCamera(Camera *);

	static void setViewMatrix(D3DXMATRIX*);
	static void setProjMatrix(D3DXMATRIX*);
	static void setMatrices(D3DXMATRIX*, D3DXMATRIX*);

	virtual D3DXVECTOR3 getPosition();

	void modVelocity(float, float, float);
	void modVelocity(D3DXVECTOR3);

	float getWidth();
	float getHeight();
	float getRotation();
	const string getIndentifier();

	void setPosition(D3DXVECTOR3);
	virtual void setPosition(float x, float y, float z);
	void setPositionY(float y);
	void modPosition(D3DXVECTOR3);
	virtual void setSize(int x, int y);
	void setVelocity(D3DXVECTOR3);
	void setVelocity(float x, float y, float z);
	void setAcceleration(D3DXVECTOR3);
	void setAcceleration(float x, float y, float z);
	void setRotation(float x);
	void modRotation(float x);
	void setID(short);
	void setCurRow(short);
	void setCurCol(short);

	virtual int update(long time);
	virtual int renderFrame(long time);
	virtual int renderFrame(long time, D3DXMATRIX * trans);

protected:
	friend class Packet;

	Quad quad;
	IDirect3DTexture9 * texture;
	ID3DXEffect * actorShader;
	
	string directory;
	short assetIndex;

	//IDirect3DIndexBuffer9 *indBuf;
	//IDirect3DVertexBuffer9 *vtxBuf;

	virtual int initGeom();

	static LPDIRECT3DDEVICE9 d3dDev;
	static AssetManager *assetManager;
	static Camera * camera;

	static D3DXMATRIX *viewMatrix;
	static D3DXMATRIX *projMatrix;

	D3DXVECTOR3 position, renderPosition;
	D3DXVECTOR3 velocity, acceleration;
	float width, height;

	short numSpriteRows, numSpriteCols, curSpriteRow, curSpriteCol;
	bool flipSprite;
	float rotation;

	short ID;
	static short currentID;

	#pragma region Shader Data
	D3DXHANDLE hWorld;
	D3DXHANDLE hMatrix;		//worldViewProj
	D3DXHANDLE hTexture;	//texture tex
	D3DXHANDLE hNumSpriteRows;
	D3DXHANDLE hNumSpriteCols;
	D3DXHANDLE hCurSpriteRow;
	D3DXHANDLE hCurSpriteCol;
	D3DXHANDLE hFlipSprite;
	D3DXHANDLE hTechnique;
	#pragma endregion

	float offset;
	string identifier;
};