#pragma once

#include "d3d9.h"
#include <d3dx9.h>
#include <string>

using std::string;

class Texture
{
private:
	friend class AssetManager;
	friend class AssetLoader;

	string name;
	IDirect3DTexture9 *texture;
};