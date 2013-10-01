#include "AllocMeshHierarchy.h"

/**
 * \brief callback called when a new frame is encountered during the .x file load
 * \param Name - name of the frame
 * \param ppNewFrame - output pointer assign our newly created frame
 * \return success code
 * \author Keith Ditchburn \date 17 July 2005
*/
HRESULT AllocMeshHierarchy::CreateFrame(LPCSTR Name, LPD3DXFRAME *retNewFrame)
{
	// Always a good idea to initialise a return pointer before proceeding
	*retNewFrame = 0;

	// Create a new frame using the derived version of the structure
    FrameEx *newFrame = new FrameEx();
	ZeroMemory(newFrame,sizeof(FrameEx));

	// Now fill in the data members in the frame structure
	
    // Now initialize other data members of the frame to defaults
    D3DXMatrixIdentity(&newFrame->TransformationMatrix);
	D3DXMatrixIdentity(&newFrame->combinedTransform);

	newFrame->pMeshContainer = 0;
    newFrame->pFrameSibling = 0;
    newFrame->pFrameFirstChild = 0;

	// Assign the return pointer to our newly created frame
    *retNewFrame = newFrame;
	
	// The frame name (note: may be 0 or zero length)
	if (Name && strlen(Name))
	{
		newFrame->Name = dupStr(Name);	
	}
	else
	{
		newFrame->Name=  "No Name";	
	}
    return S_OK;
}

/**
 * \brief callback called when a mesh data is encountered during the .x file load
 * \param Name - name of the Mesh (const char*)
 * \param meshData - the mesh data
 * \param materials - material array
 * \param effectInstances - effect files / settings for the mesh
 * \param numMaterials - number of materials in the mesh
 * \param adjacency - adjacency array 
 * \param pSkinInfo - skin info.
 * \param retNewMeshContainer - output pointer to assign our newly created mesh container
 * \return success code
 * \author Keith Ditchburn \date 17 July 2005
*/
HRESULT AllocMeshHierarchy::CreateMeshContainer(
    LPCSTR Name,
    CONST D3DXMESHDATA *meshData,
    CONST D3DXMATERIAL *materials,
    CONST D3DXEFFECTINSTANCE *effectInstances,
    DWORD numMaterials,
    CONST DWORD *adjacency,
    LPD3DXSKININFO pSkinInfo,
    LPD3DXMESHCONTAINER* retNewMeshContainer)
{    
	// Create a mesh container structure to fill and initilaise to zero values
	// Note: I use my extended version of the structure (D3DXMESHCONTAINER_EXTENDED) defined in MeshStructures.h
	D3DXMESHCONTAINER * newMeshContainer = new D3DXMESHCONTAINER;
	ZeroMemory(newMeshContainer, sizeof(D3DXMESHCONTAINER));

	// Always a good idea to initialise return pointer before proceeding
	*retNewMeshContainer = 0;

	// The mesh name (may be 0) needs copying over
	if (Name && strlen(Name))
	{
		newMeshContainer->Name= dupStr(Name);
	}
	else
	{
		newMeshContainer->Name="No Name";
	}

	// The mesh type (D3DXMESHTYPE_MESH, D3DXMESHTYPE_PMESH or D3DXMESHTYPE_PATCHMESH)
	if (meshData->Type!=D3DXMESHTYPE_MESH)
	{
		// This demo does not handle mesh types other than the standard
		// Other types are D3DXMESHTYPE_PMESH (progressive mesh) and D3DXMESHTYPE_PATCHMESH (patch mesh)
		DestroyMeshContainer(newMeshContainer);
		return E_FAIL;
	}

	newMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
	
	// Adjacency data - holds information about triangle adjacency, required by the ID3DMESH object
	DWORD dwFaces = meshData->pMesh->GetNumFaces();
	newMeshContainer->pAdjacency = new DWORD[dwFaces*3];
	memcpy(newMeshContainer->pAdjacency, adjacency, sizeof(DWORD) * dwFaces*3);
	
	// Get the Direct3D device, luckily this is held in the mesh itself (Note: must release it when done with it)
	LPDIRECT3DDEVICE9 pd3dDevice = 0;
	meshData->pMesh->GetDevice(&pd3dDevice);

	// Changed 24/09/07 - can just assign pointer and add a ref rather than need to clone
	newMeshContainer->MeshData.pMesh=meshData->pMesh;
	newMeshContainer->MeshData.pMesh->AddRef();

	// Create material and texture arrays. Note that I always want to have at least one
	newMeshContainer->NumMaterials = max(numMaterials,1);

	// If there is skin data associated with the mesh copy it over
	if (pSkinInfo)
	{
		// save off the SkinInfo
	    newMeshContainer->pSkinInfo = pSkinInfo;
	    pSkinInfo->AddRef();

	    UINT numBones = pSkinInfo->GetNumBones();
	}
	else	
	{
		newMeshContainer->pSkinInfo = 0;
	}
	
	// Set the output mesh container pointer to our newly created one
	*retNewMeshContainer = newMeshContainer;    

	return S_OK;
}

/**
 * \brief callback called to deallocate the frame data
 * \param the frame to free
 * \return success result
 * \author Keith Ditchburn \date 17 July 2005
*/
HRESULT AllocMeshHierarchy::DestroyFrame(LPD3DXFRAME frameToFree) 
{
	// Convert to our extended type. OK to do this as we know for sure it is:

	if (frameToFree->Name)
		delete []frameToFree->Name;
	delete frameToFree;

    return S_OK; 
}

/**
 * \brief callback called to deallocate the mesh container data
 * \param the mesh data to free
 * \return success result
 * \author Keith Ditchburn \date 17 July 2005
*/
HRESULT AllocMeshHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER meshContainerBase)
{
	// Convert to our extended type. OK as we know for sure it is:
	if (!meshContainerBase)
		return S_OK;

	// name
	if (meshContainerBase->Name)
	{
		delete []meshContainerBase->Name;
		meshContainerBase->Name=0;
	}
		
	// release the main mesh
	if (meshContainerBase->MeshData.pMesh)
	{
		meshContainerBase->MeshData.pMesh->Release();
		meshContainerBase->MeshData.pMesh=0;
	}
		
	// release skin information
	if (meshContainerBase->pSkinInfo)
	{
		meshContainerBase->pSkinInfo->Release();
		meshContainerBase->pSkinInfo=0;
	}
	
	// finally delete the mesh container itself
	delete meshContainerBase;
	meshContainerBase=0;

    return S_OK;
}

char* AllocMeshHierarchy::dupStr(const char* charString)
{
	if (!charString)
		return 0;

	size_t len=strlen(charString) + 1;
	char *newString = new char[len];
	memcpy(newString, charString, len*sizeof(char));

	return newString;
}
