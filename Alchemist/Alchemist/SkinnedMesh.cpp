#include "SkinnedMesh.h"

SkinnedMesh::SkinnedMesh()
{
	
}

SkinnedMesh::~SkinnedMesh()
{
	if (finalTransforms) delete[] finalTransforms;
}

void SkinnedMesh::setEffect(ID3DXEffect ** e)
{
	effect = e;
	hFinalTransforms = (*effect)->GetParameterByName(NULL, "finalTransforms");
	hNumBoneInfluences = (*effect)->GetParameterByName(NULL, "NumVertInfluences");
	finalTransforms = new D3DXMATRIX[numBones];
}

void SkinnedMesh::frameMove(float deltaTime) 
{ 
	animCtrl->AdvanceTime(deltaTime, 0);

	D3DXMATRIX identity; 
	D3DXMatrixIdentity(&identity); 
	
	combineTransforms(static_cast<FrameEx*>(root), identity); 

	D3DXMATRIX offsetTemp, combinedTemp; 
	for(UINT i = 0; i < numBones; ++i) 
	{ 
		offsetTemp = *skinInfo->GetBoneOffsetMatrix(i); 
		combinedTemp = *combinedTransforms[i]; 
		finalTransforms[i] = offsetTemp * combinedTemp; 
	} 

	updateMatrices(root, &identity);
	
}

void SkinnedMesh::updateMatrices(const D3DXFRAME *frameBase, const D3DXMATRIX *parentMatrix) 
{ 
	FrameEx *currentFrame = (FrameEx*)frameBase;

	// If parent matrix exists multiply our frame matrix by it
    if (parentMatrix != NULL)
        D3DXMatrixMultiply(&currentFrame->combinedTransform, &currentFrame->TransformationMatrix, parentMatrix);
    else
        currentFrame->combinedTransform = currentFrame->TransformationMatrix;

	// If we have a sibling recurse 
    if (currentFrame->pFrameSibling != NULL)
        updateMatrices(currentFrame->pFrameSibling, parentMatrix);

	// If we have a child recurse 
    if (currentFrame->pFrameFirstChild != NULL)
        updateMatrices(currentFrame->pFrameFirstChild, &currentFrame->combinedTransform);
}

void SkinnedMesh::combineTransforms(FrameEx* frame, D3DXMATRIX& P) 
{ 
	// Save some references to economize line space. 
	D3DXMATRIX& L = frame->TransformationMatrix; 
	D3DXMATRIX& C = frame->combinedTransform; 
	C = L * P; 
	FrameEx* sibling = (FrameEx*)frame->pFrameSibling; 
	FrameEx* firstChild = (FrameEx*)frame->pFrameFirstChild; 
	// Recurse down siblings. 
	if( sibling ) 
		combineTransforms(sibling, P); 
	// Recurse to first child. 
	if( firstChild ) 
		combineTransforms(firstChild , C); 
}

void SkinnedMesh::render(long time) 
{ 
	UINT numPasses = 0; 
	
	D3DXMATRIX identity; 
	D3DXMatrixIdentity(&identity); 
	combineTransforms(static_cast<FrameEx*>(root), identity); 

	D3DXMATRIX offsetTemp, combinedTemp; 
	for(UINT i = 0; i < numBones; ++i) 
	{ 
		offsetTemp = *(skinInfo->GetBoneOffsetMatrix(i)); 
		combinedTemp = *combinedTransforms[i]; 
		finalTransforms[i] = offsetTemp * combinedTemp; 
	} 

	(*effect)->SetMatrixArray(hFinalTransforms, &finalTransforms[0], numBones);

	(*effect)->Begin(&numPasses, 0); 
	for(UINT i = 0; i < numPasses; ++i) 
	{ 
		(*effect)->BeginPass(i); 
		mesh->DrawSubset(0); 
		(*effect)->EndPass(); 
	} 
	(*effect)->End(); 

}