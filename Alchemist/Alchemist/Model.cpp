#include "Model.h"

LPDIRECT3DDEVICE9 Model::d3dDev = NULL;

Model::Model() : mesh(0), firstMesh(0), frameRoot(0), animController(0), boneMatrices(0), maxBones(0), currentAnimationSet(0), currentTrack(0), filePath(""), name(""), skinned(true)
{}

Model::~Model()
{}

bool Model::Load(const std::string &filename)
{
	filePath = filename;

	// Create our mesh hierarchy class to control the allocation of memory - only used temporarily
	MeshHierarchy *memoryAllocator =new MeshHierarchy;

	// To make it easier to find the textures change the current directory to the one containing the .x file
	// First though remember the current one to put it back afterwards
	std::string currentDirectory=Utility::GetTheCurrentDirectory();

	std::string xfilePath;
	Utility::SplitPath(filename,&xfilePath,&name);

	//SetCurrentDirectory(xfilePath.c_str());

	// This is the function that does all the .x file loading. We provide a pointer to an instance of our 
	// memory allocator class to handle memory allocationm during the frame and loading
	HRESULT hr = D3DXLoadMeshHierarchyFromX(filename.c_str(), D3DXMESH_MANAGED, d3dDev, 
		memoryAllocator, NULL, &frameRoot, &animController);

	delete memoryAllocator;
	memoryAllocator=0;

	//SetCurrentDirectory(currentDirectory.c_str());
	
	if (Utility::FailedHr(hr))
		return false; 

	//if (frameRoot-> == 0)
	//	return false;

	// if the x file contains any animation remember how many sets there are
	if(animController)
		numAnimationSets = animController->GetMaxNumAnimationSets();
	else
		return false;

	// Bones for skining
	if(frameRoot)
	{
		// Set the bones up
		SetupBoneMatrices((D3DXFRAME_EXTENDED*)frameRoot, NULL);

		// Create the bone matrices array for use during FrameMove to hold the final transform
		boneMatrices  = new D3DXMATRIX[maxBones];
		ZeroMemory(boneMatrices, sizeof(D3DXMATRIX)*maxBones);

		// Calculate the Bounding Sphere for this model (used in CalculateInitialViewMatrix to position camera correctly)
		D3DXFrameCalculateBoundingSphere(frameRoot, &m_sphereCentre, &m_sphereRadius);
	}

	skinned = true;
	
	return true;
}

void Model::LoadBasic(LPCSTR filename)
{
	int rc = D3DXLoadMeshFromX(filename, D3DXMESH_SYSTEMMEM, d3dDev, NULL, NULL, NULL, NULL, &mesh);

	if (rc != D3D_OK)
		D3DXLoadMeshFromX("Assets\\Error.x", D3DXMESH_SYSTEMMEM, d3dDev, NULL, NULL, NULL, NULL, &mesh);

	filePath = filename;
	skinned = false;
}

void Model::SetupBoneMatrices(D3DXFRAME_EXTENDED *pFrame, LPD3DXMATRIX pParentMatrix)
{
	// Cast to our extended structure first
	D3DXMESHCONTAINER_EXTENDED* pMesh = (D3DXMESHCONTAINER_EXTENDED*)pFrame->pMeshContainer;

	// If this frame has a mesh
	if(pMesh)
	{
		// We need to remember which is the first mesh in the hierarchy for later when we 
		// update (FrameMove)
		if(!firstMesh)
			firstMesh = pMesh;
		
		// if there is skin info, then setup the bone matrices
		if(pMesh->pSkinInfo && pMesh->MeshData.pMesh)
		{
			// Create a copy of the mesh to skin into later
			D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];
			if (FAILED(pMesh->MeshData.pMesh->GetDeclaration(Declaration)))
				return;

			pMesh->MeshData.pMesh->CloneMesh(D3DXMESH_MANAGED, 
				Declaration, d3dDev, 
				&pMesh->exSkinMesh);

			// Max bones is calculated for later use (to know how big to make the bone matrices array)
			maxBones=max(maxBones,(int)pMesh->pSkinInfo->GetNumBones());

			// For each bone work out its matrix
			for (unsigned int i = 0; i < pMesh->pSkinInfo->GetNumBones(); i++)
			{   
				// Find the frame containing the bone
				D3DXFRAME_EXTENDED* pTempFrame = (D3DXFRAME_EXTENDED*)D3DXFrameFind(frameRoot, 
						pMesh->pSkinInfo->GetBoneName(i));

				// set the bone part - point it at the transformation matrix
				pMesh->exFrameCombinedMatrixPointer[i] = &pTempFrame->exCombinedTransformationMatrix;
			}
		}
	}

	// Pass on to sibblings
	if(pFrame->pFrameSibling)
		SetupBoneMatrices((D3DXFRAME_EXTENDED*)pFrame->pFrameSibling, pParentMatrix);

	// Pass on to children
	if(pFrame->pFrameFirstChild)
		SetupBoneMatrices((D3DXFRAME_EXTENDED*)pFrame->pFrameFirstChild, &pFrame->exCombinedTransformationMatrix);
}

int Model::setd3dDev(IDirect3DDevice9 *d3d_Dev)
{
	if (d3dDev == NULL)
		d3dDev = d3d_Dev;
	else
		return 1; //device is already set, cannot be re set

	return 0; //normal exit status

}