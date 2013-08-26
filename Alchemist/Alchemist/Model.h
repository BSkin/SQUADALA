#pragma once

#include "d3d9.h"
#include <d3dx9.h>
#include <string>
#include "MeshStructures.h"
#include "MeshHierarchy.h"
#include "Utility.h"

using std::string;

class Model
{
public:
	Model();
	~Model();

	static int setd3dDev(IDirect3DDevice9 *d3d_Dev);

private:
	friend class AssetManager;
	friend class AssetLoader;

	string name;
	string filePath;
	LPD3DXMESH mesh;

	bool skinned;

	LPD3DXFRAME                 frameRoot;
    LPD3DXANIMATIONCONTROLLER   animController;
	D3DXMESHCONTAINER_EXTENDED * firstMesh;

	D3DXMATRIX *boneMatrices;
	int maxBones;
	
	// Animation variables
	unsigned int currentAnimationSet;	
	unsigned int numAnimationSets;
	unsigned int currentTrack;
	//float currentTime;
	//float speedAdjust;

	static LPDIRECT3DDEVICE9 d3dDev;

	// Bounding sphere (for camera placement)
	D3DXVECTOR3 m_sphereCentre;
	float m_sphereRadius;

	//void UpdateFrameMatrices(const D3DXFRAME *frameBase, const D3DXMATRIX *parentMatrix);
	//void DrawFrame(LPD3DXFRAME frame) const;
	//void DrawMeshContainer(LPD3DXMESHCONTAINER meshContainerBase, LPD3DXFRAME frameBase) const;
	bool Load(const std::string &filename);
	void LoadBasic(LPCSTR filepath);
	void SetupBoneMatrices(D3DXFRAME_EXTENDED *pFrame, LPD3DXMATRIX pParentMatrix);	
};