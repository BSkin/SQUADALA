#pragma once

#include <d3d9.h>
#include <d3dx9.h>

class VertexCache
{
public:
	VertexCache(UINT maxVertices,UINT maxIndices,UINT stride,DWORD fvf,DWORD processing);
	virtual ~VertexCache();
	int render(long time);
private:
	HRESULT Flush();
	HRESULT Start();
	HRESULT Draw(UINT numVertices,UINT numIndices,const WORD *pIndexData,
			const void *pVertexStreamZeroData);
	DWORD m_fvf;
	UINT m_maxVertices;
	UINT m_numVertices;
	UINT m_maxIndices;
	UINT m_numIndices;
	UINT m_stride;
	BYTE *m_vertPtr;
	WORD *m_indPtr;
	IDirect3DVertexBuffer9 * vertexBuffer;
	IDirect3DIndexBuffer9 * indexBuffer;
};