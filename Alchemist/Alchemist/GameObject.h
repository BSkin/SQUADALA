#pragma once

#include <d3dx9.h>
#include "AssetManager.h"
#include "btBulletDynamicsCommon.h"

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
	GameObject(char *fileBase, D3DXVECTOR3 pos = D3DXVECTOR3(0,0,0));
	GameObject(short ID, D3DXVECTOR3 pos = D3DXVECTOR3(0,0,0));
	~GameObject(void);

	static int setd3dDev(IDirect3DDevice9 *d3d_Dev);
	static int setAssetManager(AssetManager *assetManager);

	static void setViewMatrix(D3DXMATRIX*);
	static void setProjMatrix(D3DXMATRIX*);
	static void setMatrices(D3DXMATRIX*, D3DXMATRIX*);
	static void setCameraPosition(D3DXVECTOR3);
	static void setSunDir(D3DXVECTOR3 *);
	static void setBullet(btAlignedObjectArray<btCollisionShape*> *, btDiscreteDynamicsWorld *);

	void moveForward(void);
	void moveLeft(void);
	void moveRight(void);
	void moveBackward(void);

	D3DXVECTOR3 getPosition();

	void modVelocity(float, float, float);
	void modVelocity(D3DXVECTOR3);

	void setPosition(D3DXVECTOR3);
	void setPosition(float x, float y, float z);
	void setPositionY(float y);
	void setScale(D3DXVECTOR3);
	void setScale(float x, float y, float z);
	void setVelocity(D3DXVECTOR3);
	void setVelocity(float x, float y, float z);
	void setAcceleration(D3DXVECTOR3);
	void setAcceleration(float x, float y, float z);
	void setRotation(D3DXVECTOR2);
	void setRotation(float x, float y);
	void setRotation(float x);
	void setAnimationIndex(short x);
	void setID(short);

	virtual int update(long time);
	virtual int renderFrame(long time);

protected:
	friend class Packet;

	LPD3DXMESH mesh;
	SkinnedData skinnedData;
	ID3DXEffect * actorShader;
	IDirect3DTexture9 * texture;
	
	string directory;
	short assetIndex;

	//IDirect3DIndexBuffer9 *indBuf;
	//IDirect3DVertexBuffer9 *vtxBuf;

	virtual int initGeom();
	virtual int initBullet();

	static LPDIRECT3DDEVICE9 d3dDev;
	static AssetManager *assetManager;
	static btAlignedObjectArray<btCollisionShape*> * collisionShapes;
	static btDiscreteDynamicsWorld * dynamicsWorld;

	static D3DXMATRIX *viewMatrix;
	static D3DXMATRIX *projMatrix;
	static D3DXVECTOR3 cameraPosition;

	static D3DXVECTOR3 * sunDir;

	D3DXMATRIX worldMatrix;
	D3DXMATRIX worldViewProj;

	float collisionRadius;

	D3DXVECTOR3 forwardVector;
	D3DXVECTOR3 upVector;
	D3DXVECTOR3 sideVector;
	D3DXVECTOR3 position;
	D3DXVECTOR3 velocity, acceleration;
	D3DXVECTOR3 scale;
	float horizontalRotation;
	float verticalRotation;
	bool animated;

	btBoxShape* colShape;
	btRigidBody * body;
	bool physInit;

	short animationIndex;
	short ID;
	static short currentID;
	D3DXMATRIX finalTransforms[35];

	#pragma region Shader Data
	D3DXHANDLE hWorld;
	D3DXHANDLE hMatrix;	//float4x4 worldMat
	D3DXHANDLE hTexture;	//texture tex
	D3DXHANDLE hNormal;		//float3 normal
	D3DXHANDLE hTechnique;
	D3DXHANDLE hFinalTransforms;
	D3DXHANDLE hAnimated;
	D3DXHANDLE hSunDir;
	#pragma endregion

	float offset;
};