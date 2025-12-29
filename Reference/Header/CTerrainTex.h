#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainTex : public CVIBuffer
{
protected:
	explicit CTerrainTex();
	explicit CTerrainTex(LPDIRECT3DDEVICE9	pGraphicDev);
	explicit CTerrainTex(const CTerrainTex& rhs);
	virtual ~CTerrainTex();

public:
	const _vec3* Get_VtxPos() { return m_pPos; }

public:
	HRESULT			Ready_Buffer(	const _ulong& dwCntX, 
									const _ulong& dwCntZ, 
									const _ulong& dwVtxItv);

	virtual void	Render_Buffer();

private:
	HANDLE				m_hFile;
	BITMAPFILEHEADER	m_fH;
	BITMAPINFOHEADER	m_iH;
	_vec3*				m_pPos;

public:
	static CTerrainTex* Create(	LPDIRECT3DDEVICE9 pGraphicDev, 
								const _ulong& dwCntX = VTXCNTX,
								const _ulong& dwCntZ = VTXCNTZ,
								const _ulong& dwVtxItx = VTXITV);

	virtual CComponent* Clone();
private:
	virtual		void	Free();
};

END
