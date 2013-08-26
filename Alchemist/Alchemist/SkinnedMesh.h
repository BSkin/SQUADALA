#pragma once

#include "d3dx9.h"

class SkinnedMesh
{
public:
	SkinnedMesh();
	~SkinnedMesh();

	void setEffect(ID3DXEffect **);
	void frameMove(float deltaTime);
	void render(long time);

	struct FrameEx : public D3DXFRAME {	D3DXMATRIX combinedTransform; };

private:
	friend class AssetManager;
	void combineTransforms(FrameEx* frame, D3DXMATRIX& P);
	void updateMatrices(const D3DXFRAME *frameBase, const D3DXMATRIX *parentMatrix);

	//Get From SkinnedModel class
	LPD3DXMESH mesh;
	ID3DXSkinInfo * skinInfo;
	DWORD maxVertInfluences;
	int numBones;
	D3DXMATRIX * boneOffsets;
	D3DXMATRIX * * combinedTransforms;
	D3DXFRAME * root;
	LPD3DXANIMATIONCONTROLLER animCtrl;

	D3DXMATRIX * finalTransforms;
	ID3DXEffect ** effect;

	D3DXHANDLE hFinalTransforms;
	D3DXHANDLE hNumBoneInfluences;
};