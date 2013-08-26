#include "AssetLoader.h"

IDirect3DDevice9 *AssetLoader::d3dDev = NULL;

AssetLoader::AssetLoader() : pobjectsToLoad(0), pmodels(0), ptextures(0), peffects(0)
{
	int x = 2;
}

AssetLoader::~AssetLoader()
{}

void AssetLoader::start()
{
	while (true)
	{
		if (!pobjectsToLoad->empty())
		{
			string s = pobjectsToLoad->front();
			if (s != "")
			{
				string tmp = s;
				string backwards = "";
				while (s.back() != '.') 
				{
					backwards += s.back();
					s.pop_back();
				}
				backwards += '.';
				string ext;
				while (!backwards.empty())
				{
					ext += backwards.back();
					backwards.pop_back();
				}

				int x = 2;
	
				if (ext.compare(".x") == 0)
					loadModel(tmp.c_str());
				else if (ext.compare(".png") == 0 || ext.compare(".dds") == 0 || ext.compare(".jpg") == 0)
					loadTexture(tmp.c_str());
				else if (ext.compare(".hlsl") == 0)
					loadEffect(tmp.c_str());
	
				pobjectsToLoad->pop_front();
			}
			else if (s == "") pobjectsToLoad->pop_front(); 
		}
		else Sleep(1000); //nothing to load, wait 1 second before checking again
	}
}

void AssetLoader::loadModel(LPCSTR path)
{
	Model * tempModel;
	tempModel = new Model();
	
	HRESULT hr = tempModel->Load(path);
	if (!hr)
		tempModel->LoadBasic(path);

	pmodels->push_back(tempModel);
}

void AssetLoader::loadTexture(LPCSTR path)
{
	IDirect3DTexture9 * tempTex;

	int rc = D3DXCreateTextureFromFileA(d3dDev, path, &tempTex);
	if (rc != D3D_OK)
		D3DXCreateTextureFromFileA(d3dDev, "Assets\\Error.png", &tempTex);
	
	Texture * tempTexture = new Texture();
	tempTexture->name = path;
	tempTexture->texture = tempTex;
	ptextures->push_back(tempTexture);
}

void AssetLoader::loadEffect(LPCSTR path)
{
	ID3DXEffect * tempFX;
	ID3DXBuffer *errors = NULL;

	string tech = path;
	
	while (tech.back() != '.')
		tech.pop_back();
	
	tech.pop_back(); //.
	tech += string("Technique");

	int rc = D3DXCreateEffectFromFile(d3dDev, 
		path, 
		0, 0, D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION, 0, 
		&tempFX, 
		&errors);
	
	if (errors != NULL) 
	{
		MessageBox(0, (char*) errors->GetBufferPointer(),0,0);
		errors->Release();
	}
	else
	{
		Effect * tempEffect = new Effect();
		tempEffect->name = path;
		tempEffect->techniqueName = tech;
		tempEffect->inUse = false;
		tempEffect->effect = tempFX;
		peffects->push_back(tempEffect);
	}
}

int AssetLoader::setd3dDev(IDirect3DDevice9 *d3d_Dev)
{
	if (d3dDev == NULL)
		d3dDev = d3d_Dev;
	else
		return 1; //device is already set, cannot be re set

	return 0; //normal exit status

}