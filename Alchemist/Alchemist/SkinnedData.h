#include <d3d9.h>
#include <d3dx9.h>
#include "MeshStructures.h"
#include <iostream>

class SkinnedData
{
public:
	SkinnedData();
	~SkinnedData();

	bool Load(const std::string &filename);
	void FrameMove(float elapsedTime,const D3DXMATRIX *matWorld);
	
	void Render() const;
	void SetAnimationSet(unsigned int index);

	void NextAnimation();
	void AnimateFaster();
	void AnimateSlower();

	D3DXVECTOR3 GetInitialCameraPosition() const;
	unsigned int GetCurrentAnimationSet() const {return currentAnimationSet;}
	std::string GetAnimationSetName(unsigned int index);
	std::string GetFilename() const {return filepath;}

	static int setd3dDev(LPDIRECT3DDEVICE9 d3d_Dev);
	bool isNull();

private:
	static LPDIRECT3DDEVICE9 d3dDevice;

	friend class AssetManager;

	// Direct3D objects required for animation
	LPD3DXFRAME                 frameRoot;
    LPD3DXANIMATIONCONTROLLER   animController;
	D3DXMESHCONTAINER_EXTENDED * firstMesh;

	// Bone data
	D3DXMATRIX *boneMatrices;
	int maxBones;
	
	// Animation variables
	unsigned int currentAnimationSet;	
	unsigned int numAnimationSets;
	unsigned int currentTrack;
	float currentTime;
	float speedAdjust;

	// Bounding sphere (for camera placement)
	D3DXVECTOR3 sphereCentre;
	float sphereRadius;

	std::string filepath;

	void UpdateFrameMatrices(const D3DXFRAME *frameBase, const D3DXMATRIX *parentMatrix);
	void DrawFrame(LPD3DXFRAME frame) const;
	void DrawMeshContainer(LPD3DXMESHCONTAINER meshContainerBase, LPD3DXFRAME frameBase) const;
	void SetupBoneMatrices(D3DXFRAME_EXTENDED *pFrame, LPD3DXMATRIX pParentMatrix);	
};