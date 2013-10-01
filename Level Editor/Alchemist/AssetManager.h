#pragma once

#include "d3d9.h"

#include "Texture.h"
#include "Effect.h"

//class AssetLoader;
#include "AssetLoader.h"

//#include "AllocMeshHierarchy.h"
#include <string>
#include <list>

using std::string;
using std::list;

#define ASSET_COUNT 7

class AssetManager
{
public:
	AssetManager();
	~AssetManager();
	
	HRESULT getTexture(
		LPCSTR path, 
		IDirect3DTexture9 ** texture);
	HRESULT getTexture(
		short ID,
		IDirect3DTexture9 ** texture);

	HRESULT getEffect(
		LPCSTR path, 
		ID3DXEffect ** effect);

	string getEffectTechnique(LPCSTR path);

	static int setd3dDev(IDirect3DDevice9 *d3d_Dev);

	short getAssetIndex(LPCSTR path);
	LPCSTR getAssetPath(short ID);
	
	list<string> * getObjectToLoadAddress();
	list<Texture *> * getTextureAddress();
	list<Effect *> * getEffectAddress();

private:
	static LPDIRECT3DDEVICE9 d3dDev;
	
	void initLookUp();

	list<string> objectsToLoad;

	list<Texture *> textures;
	list<Effect *> effects;

	string * assetPaths; 

	int loadAsset(LPCSTR path);

	int textureListContains(LPCSTR);
	int effectListContains(LPCSTR);

	int objectListContains(LPCSTR);

	IDirect3DTexture9 * getTextureFromList(LPCSTR);
	ID3DXEffect * getEffectFromList(LPCSTR);
	string getEffectTechniqueFromList(LPCSTR);
};