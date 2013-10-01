#pragma once

#include "d3dx9.h"
#include <string>

class AllocMeshHierarchy : public ID3DXAllocateHierarchy 
{
public:	
    // callback to create a D3DXFRAME extended object and initialize it
    STDMETHOD( CreateFrame )(LPCSTR Name, LPD3DXFRAME *retNewFrame );

    // callback to create a D3DXMESHCONTAINER extended object and initialize it
    STDMETHOD( CreateMeshContainer )(LPCSTR Name, CONST D3DXMESHDATA * meshData, 
                            CONST D3DXMATERIAL * materials, CONST D3DXEFFECTINSTANCE * effectInstances,
                            DWORD numMaterials, CONST DWORD * adjacency, LPD3DXSKININFO skinInfo, 
                            LPD3DXMESHCONTAINER * retNewMeshContainer );

    // callback to release a D3DXFRAME extended object
    STDMETHOD( DestroyFrame )(LPD3DXFRAME frameToFree );

    // callback to release a D3DXMESHCONTAINER extended object
    STDMETHOD( DestroyMeshContainer )(LPD3DXMESHCONTAINER meshContainerToFree );

private:
	struct FrameEx : public D3DXFRAME {	D3DXMATRIX combinedTransform; };
	char* dupStr(const char* charString);
};