#include "AssetManager.h"

using std::find;

IDirect3DDevice9 *AssetManager::d3dDev = NULL;

AssetManager::AssetManager() : models(), textures(), effects(), assetPaths(NULL)
{
	
}

AssetManager::~AssetManager()
{
	while (models.size() > 0)
	{
		if (models.front()->mesh) 
			models.front()->mesh->Release();

		if (models.front()->animController)
		{
			models.front()->animController->Release();
			models.front()->animController=0;
		}

		if (models.front()->frameRoot)
		{
			//Create a mesh heirarchy class to control the removal of memory for the frame heirarchy
			MeshHierarchy memoryAllocator;
			D3DXFrameDestroy(models.front()->frameRoot, &memoryAllocator);
			models.front()->frameRoot=0;
		}

		if (models.front()->boneMatrices)
		{
			delete [] models.front()->boneMatrices;
			models.front()->boneMatrices=0;
		}
		models.pop_front();
	}
	while (textures.size() > 0)
	{
		if (textures.front()->texture)
			textures.front()->texture->Release();
		textures.pop_front();
	}
	while (effects.size() > 0)
	{
		if (effects.front()->effect)
			effects.front()->effect->Release();
		effects.pop_front();
	}
	int x = 2;
}

#pragma region Get Functions
HRESULT AssetManager::getModel(LPCSTR path, LPD3DXMESH * mesh, SkinnedData * skinnedData)
{
	if (modelListContains(path) == 0)
		loadAsset(path);
	Model * temp = getModelFromList(path);

	if (temp)
	{
		if (temp->skinned)
		{
			temp->animController->CloneAnimationController(
				temp->animController->GetMaxNumAnimationOutputs(),
				temp->animController->GetMaxNumAnimationSets(),
				temp->animController->GetMaxNumTracks(),
				temp->animController->GetMaxNumEvents(),
				&skinnedData->animController);
				
			(*skinnedData).frameRoot = temp->frameRoot;
			(*skinnedData).firstMesh = temp->firstMesh;
		
			(*skinnedData).numAnimationSets = temp->numAnimationSets;
			(*skinnedData).maxBones = temp->maxBones;

			(*skinnedData).boneMatrices  = new D3DXMATRIX[(*skinnedData).maxBones];
			ZeroMemory((*skinnedData).boneMatrices, sizeof(D3DXMATRIX)*(*skinnedData).maxBones);

			int x = 2;
		}
		else
		{
			*mesh = temp->mesh;
		}
		int x = 2;

		return D3D_OK;
	}

	return -1;
}
HRESULT AssetManager::getModel(short ID, LPD3DXMESH * mesh, SkinnedData * skinnedData)
{
	return getModel(getAssetPath(ID), mesh, skinnedData);
}

HRESULT AssetManager::getTexture(LPCSTR path, IDirect3DTexture9 ** texture)
{
	if (textureListContains(path) == 0)
		loadAsset(path);
	*texture = getTextureFromList(path);
	
	if (*texture) return D3D_OK;
	return -1;
}
HRESULT AssetManager::getTexture(short ID, IDirect3DTexture9 ** texture)
{
	return getTexture(getAssetPath(ID), texture);
}

HRESULT AssetManager::getEffect(LPCSTR path, ID3DXEffect ** effect)
{
	if (effectListContains(path) == 0)
		loadAsset(path);
	*effect = getEffectFromList(path);
	if (*effect) return D3D_OK;
	return -1;
}

string AssetManager::getEffectTechnique(LPCSTR path)
{
	return getEffectTechniqueFromList(path);
}
#pragma endregion

#pragma region Load Functions
int AssetManager::loadAsset(LPCSTR path)
{	
	if (objectListContains(path)) return 0;
	else objectsToLoad.push_back(path);

	return -1;
}
#pragma endregion

#pragma region Contains functions
int AssetManager::objectListContains(LPCSTR item)
{
	list<string>::iterator iter;

	for (iter = objectsToLoad.begin(); iter != objectsToLoad.end(); iter++)
		if ((*iter).compare(item) == 0) return 1;

	return 0;
}

int AssetManager::modelListContains(LPCSTR item)
{
	list<Model *>::iterator iter;
	
	for (iter = models.begin(); iter != models.end(); iter++)
		if ((*iter)->filePath == item) return 1;

	return 0;
}

int AssetManager::textureListContains(LPCSTR item)
{
	//if (textures.size() == 0) return 0;

	if (textures.empty()) return 0;

	list<Texture *>::iterator iter;
	
	for (iter = textures.begin(); iter != textures.end(); iter++)
		if ((*iter)->name == item) return 1;

	return 0;
}

int AssetManager::effectListContains(LPCSTR item)
{
	list<Effect *>::iterator iter;
	
	for (iter = effects.begin(); iter != effects.end(); iter++)
		//if ((*iter)->name == item && (*iter)->inUse == false) return 1;
		if ((*iter)->name == item) return 1;

	return 0;
}
#pragma endregion

#pragma region Retrieve From List Functions
Model * AssetManager::getModelFromList(LPCSTR item)
{
	list<Model *>::iterator iter;
	
	for (iter = models.begin(); iter != models.end(); iter++)
	{
		if ((*iter)->filePath == item) return *iter;
	}

	return 0;
}

IDirect3DTexture9 * AssetManager::getTextureFromList(LPCSTR item)
{
	list<Texture *>::iterator iter;
	
	for (iter = textures.begin(); iter != textures.end(); iter++)
	{
		if ((*iter)->name == item) return (*iter)->texture;
	}

	return 0;
}

ID3DXEffect * AssetManager::getEffectFromList(LPCSTR item)
{

	list<Effect *>::iterator iter;
	
	for (iter = effects.begin(); iter != effects.end(); iter++)
	{
		if ((*iter)->name == item)
		//if ((*iter)->name == item && (*iter)->inUse == false) 
		{
			//(*iter)->inUse = true;
			return (*iter)->effect;
		}
	}

	return 0;
}

string AssetManager::getEffectTechniqueFromList(LPCSTR item)
{
	list<Effect *>::iterator iter;
	
	for (iter = effects.begin(); iter != effects.end(); iter++)
	{
		if ((*iter)->name == item) return (*iter)->techniqueName;
	}

	return 0;
}
#pragma endregion

list<string> * AssetManager::getObjectToLoadAddress() { return &objectsToLoad; }
list<Model *> * AssetManager::getModelAddress() { return &models; }
list<Texture *> * AssetManager::getTextureAddress() { return &textures; }
list<Effect *> * AssetManager::getEffectAddress() { return &effects; }

short AssetManager::getAssetIndex(LPCSTR path)
{
	if (assetPaths == NULL) //First time check, set up the lookup array
		initLookUp();

	for (short i = 0; i < ASSET_COUNT; i++)
	{
		if (assetPaths[i] == path) return i;
	}

	string temp = path;
	string s = "";
	while (temp.back() != '.')
	{
		s += temp.back();
		temp.pop_back();
	}

	if (s == "gnp" || s == "sdd") return 0;
	else if (s == "x") return 1;
}

LPCSTR AssetManager::getAssetPath(short ID)
{
	if (assetPaths == NULL)
		initLookUp();

	if (ID >= ASSET_COUNT || ID < 0) return "";

	return assetPaths[ID].c_str();
}

void AssetManager::initLookUp()
{
	assetPaths = new string[ASSET_COUNT];
	assetPaths[0] = "Assets\\Error.png";
	assetPaths[1] = "Assets\\Error.x";
	assetPaths[2] = "Assets\\TitleTemp.png";
	assetPaths[3] = "Assets\\HealthTemp.png";
	assetPaths[4] = "Assets\\cube.x";
	assetPaths[5] = "Assets\\stonesword.x";
	assetPaths[6] = "Assets\\witchdoctor.x";
}

int AssetManager::setd3dDev(IDirect3DDevice9 *d3d_Dev)
{
	if (d3dDev == NULL)
		d3dDev = d3d_Dev;
	else
		return 1; //device is already set, cannot be re set

	return 0; //normal exit status

}