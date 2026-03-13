#pragma once
#include "CComponent.h"

BEGIN(Engine)

class CTerrainTex;
class CTransform;

class ENGINE_DLL CCalculator : public CComponent
{
private:
	explicit	CCalculator(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual		~CCalculator();

public:
	HRESULT		Ready_Calculator();

	_float		Compute_HeightOnTerrain(const _vec3* pPos,
										const _vec3* pTerrainVtxPos,
										const _ulong& dwCntX,
										const _ulong& dwCntZ);

	_vec3		Picking_OnTerrain(	HWND hWnd,
									CTerrainTex* pTerrainBufferCom,
									CTransform* pTerrainTransformCom);

public:
	static CCalculator*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	CComponent*	Clone() override;

private:
	void	Free() override;
};
END

