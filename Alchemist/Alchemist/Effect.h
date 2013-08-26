#pragma once

#include "d3d9.h"
#include <d3dx9.h>
#include <string>

using std::string;

class Effect
{
private:
	friend class AssetManager;
	friend class AssetLoader;

	string name, techniqueName;
	bool inUse;
	ID3DXEffect *effect;
};