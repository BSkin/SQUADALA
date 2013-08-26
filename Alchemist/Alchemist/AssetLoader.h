#pragma once

#include "Model.h"
#include "Texture.h"
#include "Effect.h"

#include <list>

using std::list;



class AssetLoader
{
public:
	AssetLoader();
	~AssetLoader();

	void start();
	static int setd3dDev(IDirect3DDevice9 *d3d_Dev);

	list<string> * pobjectsToLoad;
	
	list<Model *> * pmodels;
	list<Texture *> * ptextures;
	list<Effect *> * peffects;

private:
	friend class AssetManager;

	static LPDIRECT3DDEVICE9 d3dDev; 

	void loadModel(LPCSTR path);
	void loadTexture(LPCSTR path);
	void loadEffect(LPCSTR path);
};