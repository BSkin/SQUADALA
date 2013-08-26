#include "SkinnedData.h"
#include "Utility.h"
#include "MeshHierarchy.h"

LPDIRECT3DDEVICE9 SkinnedData::d3dDevice = NULL;

// The time to change from one animation set to another
// To see how the merging works - increase this time value to slow it down
const float kMoveTransitionTime=0.25f;

// Constructor
SkinnedData::SkinnedData() : speedAdjust(1.0f),firstMesh(0),
	currentTrack(0),currentTime(0),numAnimationSets(0),currentAnimationSet(0),maxBones(0),sphereRadius(0),
	sphereCentre(0,0,0),boneMatrices(0)
{
	animController = 0;
	frameRoot = 0;
	firstMesh = 0;
	boneMatrices = 0;
}

// Destructor
SkinnedData::~SkinnedData(void)
{
	if (animController)
	{
		animController->Release();
		animController=0;
	}

	/*if (frameRoot)
	{
		// Create a mesh heirarchy class to control the removal of memory for the frame heirarchy
		MeshHierarchy memoryAllocator;
		D3DXFrameDestroy(frameRoot, &memoryAllocator);
		frameRoot=0;
	}*/

	if (boneMatrices)
	{
		delete []boneMatrices;
		boneMatrices=0;
	}
}

/*
	Load the x file
	The function D3DXLoadMeshHierarchyFromX requires a support object to handle memeory allocation etc.
	I have defined this in the class CMeshHierarchy
*/
bool SkinnedData::Load(const std::string &filename)
{
	

	return true;
}

/**
 * \brief Called each frame update with the time and the current world matrix
 * \param elapsedTime - time passed
 * \param matWorld - current world matrix for the model
 * \author Keith Ditchburn \date 18 July 2005
*/
void SkinnedData::FrameMove(float elapsedTime,const D3DXMATRIX *matWorld)
{
	// Adjust animation speed
	elapsedTime/=speedAdjust;

	// Advance the time and set in the controller
    if (animController != NULL)
        animController->AdvanceTime(elapsedTime, NULL);

	currentTime+=elapsedTime;

	// Now update the model matrices in the hierarchy
    UpdateFrameMatrices(frameRoot, matWorld);

	// If the model contains a skinned mesh update the vertices
	D3DXMESHCONTAINER_EXTENDED* pMesh = firstMesh;
	if(pMesh && pMesh->pSkinInfo)
	{
		unsigned int Bones = pMesh->pSkinInfo->GetNumBones();

		// Create the bone matrices that transform each bone from bone space into character space
		// (via exFrameCombinedMatrixPointer) and also wraps the mesh around the bones using the bone offsets
		// in exBoneOffsetsArray
		for (unsigned int i = 0; i < Bones; ++i)
			D3DXMatrixMultiply(&boneMatrices[i],&pMesh->exBoneOffsets[i], pMesh->exFrameCombinedMatrixPointer[i]);

		// We need to modify the vertex positions based on the new bone matrices. This is achieved
		// by locking the vertex buffers and then calling UpdateSkinnedMesh. UpdateSkinnedMesh takes the
		// original vertex data (in pMesh->MeshData.pMesh), applies the matrices and writes the new vertices
		// out to skin mesh (pMesh->exSkinMesh). 

		// UpdateSkinnedMesh uses software skinning which is the slowest way of carrying out skinning 
		// but is easiest to describe and works on the majority of graphic devices. 
		// Other methods exist that use hardware to do this skinning - see the notes and the 
		// DirectX SDK skinned mesh sample for more details
		void *srcPtr=0;
		pMesh->MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&srcPtr);

		void *destPtr=0;
		pMesh->exSkinMesh->LockVertexBuffer(0, (void**)&destPtr);

		// Update the skinned mesh 
		pMesh->pSkinInfo->UpdateSkinnedMesh(boneMatrices, NULL, srcPtr, destPtr);

		// Unlock the meshes vertex buffers
		pMesh->exSkinMesh->UnlockVertexBuffer();
		pMesh->MeshData.pMesh->UnlockVertexBuffer();
	}
}

/**
 * \brief Called to update the frame matrices in the hierarchy to reflect current animation stage
 * \param frameBase - frame being looked at
 * \param parentMatrix - the matrix of our parent (if we have one)
 * \author Keith Ditchburn \date 18 July 2005
*/
void SkinnedData::UpdateFrameMatrices(const D3DXFRAME *frameBase, const D3DXMATRIX *parentMatrix)
{
    D3DXFRAME_EXTENDED *currentFrame = (D3DXFRAME_EXTENDED*)frameBase;

	// If parent matrix exists multiply our frame matrix by it
    if (parentMatrix != NULL)
        D3DXMatrixMultiply(&currentFrame->exCombinedTransformationMatrix, &currentFrame->TransformationMatrix, parentMatrix);
    else
        currentFrame->exCombinedTransformationMatrix = currentFrame->TransformationMatrix;

	// If we have a sibling recurse 
    if (currentFrame->pFrameSibling != NULL)
        UpdateFrameMatrices(currentFrame->pFrameSibling, parentMatrix);

	// If we have a child recurse 
    if (currentFrame->pFrameFirstChild != NULL)
        UpdateFrameMatrices(currentFrame->pFrameFirstChild, &currentFrame->exCombinedTransformationMatrix);
}

/**
 * \brief Render our mesh.
 * Call the DrawFrame recursive fn on render with the root frame (see notes diagram)
 * \param device - the Direct3D device object
 * \author Keith Ditchburn \date 18 July 2005
*/
void SkinnedData::Render() const
{
	if (frameRoot)
		DrawFrame(frameRoot);
}

/**
 * \brief Called to render a frame in the hierarchy
 * \param device - the Direct3D device object
 * \param frame - frame to render
 * \author Keith Ditchburn \date 18 July 2005
*/
void SkinnedData::DrawFrame(LPD3DXFRAME frame) const
{
	// Draw all mesh containers in this frame
    LPD3DXMESHCONTAINER meshContainer = frame->pMeshContainer;
    while (meshContainer)
    {
        DrawMeshContainer(meshContainer, frame);
        meshContainer = meshContainer->pNextMeshContainer;
    }

	// Recurse for sibblings
    if (frame->pFrameSibling != NULL)
        DrawFrame(frame->pFrameSibling);

    // Recurse for children
	if (frame->pFrameFirstChild != NULL)
        DrawFrame(frame->pFrameFirstChild);
}

/**
 * \brief Called to render a mesh
 * \param device - the Direct3D device object
 * \param meshContainerBase - the mesh container
 * \param frameBase - frame containing the mesh
 * \author Keith Ditchburn \date 18 July 2005
*/
void SkinnedData::DrawMeshContainer(LPD3DXMESHCONTAINER meshContainerBase, LPD3DXFRAME frameBase) const
{
	// Cast to our extended frame type
	D3DXFRAME_EXTENDED *frame = (D3DXFRAME_EXTENDED*)frameBase;		

	// Cast to our extended mesh container
	D3DXMESHCONTAINER_EXTENDED *meshContainer = (D3DXMESHCONTAINER_EXTENDED*)meshContainerBase;
	
	// Set the world transform
    d3dDevice->SetTransform(D3DTS_WORLD, &frame->exCombinedTransformationMatrix);

	// Loop through all the materials in the mesh rendering each subset
    for (unsigned int iMaterial = 0; iMaterial < meshContainer->NumMaterials; iMaterial++)
    {
		// Select the mesh to draw, if there is skin then use the skinned mesh else the normal one
		LPD3DXMESH pDrawMesh = (meshContainer->pSkinInfo) ? meshContainer->exSkinMesh: meshContainer->MeshData.pMesh;

		// Finally Call the mesh draw function
        pDrawMesh->DrawSubset(iMaterial);
    }
}

/**
 * \brief Change to a different animation set
 * Handles transitions between animations to make it smooth and not a sudden jerk to a new position
 * \param index - new animation set index
 * \author Keith Ditchburn \date 18 July 2005
*/
void SkinnedData::SetAnimationSet(unsigned int index)
{
	if (index==currentAnimationSet)
		return;

	if (index>=numAnimationSets)
		index=0;

	// Remember current animation
	currentAnimationSet=index;

	// Get the animation set from the controller
	LPD3DXANIMATIONSET set;
	animController->GetAnimationSet(currentAnimationSet, &set );	

	// Note: for a smooth transition between animation sets we can use two tracks and assign the new set to the track
	// not currently playing then insert Keys into the KeyTrack to do the transition between the tracks
	// tracks can be mixed together so we can gradually change into the new animation

	// Alternate tracks
	DWORD newTrack = ( currentTrack == 0 ? 1 : 0 );

	// Assign to our track
	animController->SetTrackAnimationSet( newTrack, set );
    set->Release();	

	// Clear any track events currently assigned to our two tracks
	animController->UnkeyAllTrackEvents( currentTrack );
    animController->UnkeyAllTrackEvents( newTrack );

	// Add an event key to disable the currently playing track kMoveTransitionTime seconds in the future
    animController->KeyTrackEnable( currentTrack, FALSE, currentTime + kMoveTransitionTime );
	// Add an event key to change the speed right away so the animation completes in kMoveTransitionTime seconds
    animController->KeyTrackSpeed( currentTrack, 0.0f, currentTime, kMoveTransitionTime, D3DXTRANSITION_LINEAR );
	// Add an event to change the weighting of the current track (the effect it has blended with the secon track)
    animController->KeyTrackWeight( currentTrack, 0.0f, currentTime, kMoveTransitionTime, D3DXTRANSITION_LINEAR );

	// Enable the new track
    animController->SetTrackEnable( newTrack, TRUE );
	// Add an event key to set the speed of the track
    animController->KeyTrackSpeed( newTrack, 1.0f, currentTime, kMoveTransitionTime, D3DXTRANSITION_LINEAR );
	// Add an event to change the weighting of the current track (the effect it has blended with the first track)
	// As you can see this will go from 0 effect to total effect(1.0f) in kMoveTransitionTime seconds and the first track goes from 
	// total to 0.0f in the same time.
    animController->KeyTrackWeight( newTrack, 1.0f, currentTime, kMoveTransitionTime, D3DXTRANSITION_LINEAR );

	// Remember current track
    currentTrack = newTrack;
}

/**
 * \brief Go to the next animation
 * \author Keith Ditchburn \date 18 July 2005
*/
void SkinnedData::NextAnimation()
{	
	unsigned int newAnimationSet=currentAnimationSet+1;
	if (newAnimationSet>=numAnimationSets)
		newAnimationSet=0;

	SetAnimationSet(newAnimationSet);
}

/**
 * \brief Get the name of the animation
 * Note: altered 24/09/07 to solve a D3DX memory leak caused because I was not releasing the set after getting it
 * \param index - the animation set index
 * \return the name
 * \author Keith Ditchburn \date 18 July 2005
*/
std::string SkinnedData::GetAnimationSetName(unsigned int index)
{
	if (index>=numAnimationSets)
		return "Error: No set exists";

	// Get the animation set
	LPD3DXANIMATIONSET set;
	animController->GetAnimationSet(currentAnimationSet, &set );

	std::string nameString(set->GetName());

	set->Release();

	return nameString;
}

/**
 * \brief Slow down animation
 * \author Keith Ditchburn \date 18 July 2005
*/
void SkinnedData::AnimateSlower()
{
	speedAdjust+=0.1f;
}

/**
 * \brief Speed up animation
 * \author Keith Ditchburn \date 18 July 2005
*/
void SkinnedData::AnimateFaster()
{
	if (speedAdjust>0.1f)
		speedAdjust-=0.1f;
}

int SkinnedData::setd3dDev(IDirect3DDevice9 *d3d_Dev)
{
	if (d3dDevice == NULL)
		d3dDevice = d3d_Dev;
	else
		return 1; //device is already set, cannot be re set

	return 0; //normal exit status

}

bool SkinnedData::isNull()
{
	if (frameRoot == 0) return true;
}