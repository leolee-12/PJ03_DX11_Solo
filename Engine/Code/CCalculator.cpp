#include "CCalculator.h"
#include "CTerrainTex.h"
#include "CTransform.h"

CCalculator::CCalculator(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
}

CCalculator::~CCalculator()
{
}

HRESULT CCalculator::Ready_Calculator()
{
	return S_OK;
}

_float CCalculator::Compute_HeightOnTerrain(const _vec3* pPos, const _vec3* pTerrainVtxPos, const _ulong& dwCntX, const _ulong& dwCntZ)
{
	_ulong		dwIndex = _ulong(pPos->z / 1.f) * dwCntX + _ulong(pPos->x / 1.f);

	_float		fRatioX = (pPos->x - pTerrainVtxPos[dwIndex + dwCntX].x) / 1.f;
	_float		fRatioZ = (pTerrainVtxPos[dwIndex + dwCntX].z - pPos->z) / 1.f;

	D3DXPLANE	Plane;

	if (fRatioX > fRatioZ)
	{
		D3DXPlaneFromPoints(&Plane,
							&pTerrainVtxPos[dwIndex + dwCntX],
							&pTerrainVtxPos[dwIndex + dwCntX + 1],
							&pTerrainVtxPos[dwIndex + 1]);

	}
	else
	{
		D3DXPlaneFromPoints(&Plane,
							&pTerrainVtxPos[dwIndex + dwCntX],
							&pTerrainVtxPos[dwIndex + 1],
							&pTerrainVtxPos[dwIndex]);
	}

	return (-Plane.a * pPos->x - Plane.c * pPos->z - Plane.d) / Plane.b;
}

_vec3 CCalculator::Picking_OnTerrain(HWND hWnd, CTerrainTex* pTerrainBufferCom, CTransform* pTerrainTransformCom)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_vec3   vMousePos;

	D3DVIEWPORT9	ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

	m_pGraphicDev->GetViewport(&ViewPort);

	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	// 투영 -> 뷰 스페이스
	D3DXMATRIX      matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, 0, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	// 뷰 스페이스 -> 월드 스페이스
	D3DXMATRIX      matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);

	_vec3       vRayPos{ 0.f, 0.f, 0.f };		// 뷰 스페이스
	_vec3       vRayDir = vMousePos - vRayPos;	// 뷰 스페이스
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	// 월드 스페이스 -> 로컬 스페이스
	_matrix matWorld = *(pTerrainTransformCom->Get_World());
	D3DXMatrixInverse(&matWorld, 0, &matWorld);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);

	const _vec3* pTerrainVtxPos = pTerrainBufferCom->Get_VtxPos();
	_float fU(0.f), fV(0.f), fDist(0.f);
	_ulong dwVtxNumber[3]{};

	for (_ulong i = 0; i < VTXCNTZ - 1; ++i)
	{
		for (_ulong j = 0; j < VTXCNTX - 1; ++j)
		{
			_ulong dwIndex = i * VTXCNTX + j;

			dwVtxNumber[0] = dwIndex + VTXCNTX;
			dwVtxNumber[1] = dwIndex + VTXCNTX + 1;
			dwVtxNumber[2] = dwIndex + 1;

			if (D3DXIntersectTri(&pTerrainVtxPos[dwVtxNumber[1]],
				&pTerrainVtxPos[dwVtxNumber[0]],
				&pTerrainVtxPos[dwVtxNumber[2]],
				&vRayPos, &vRayDir,
				&fU, &fV, &fDist))
			{
				// V1 + U(V2 - V1) + V(V3 - V1)
				return _vec3(pTerrainVtxPos[dwVtxNumber[1]].x + fU * (pTerrainVtxPos[dwVtxNumber[0]].x - pTerrainVtxPos[dwVtxNumber[1]].x),
					0.f,
					pTerrainVtxPos[dwVtxNumber[1]].z + fV * (pTerrainVtxPos[dwVtxNumber[2]].z - pTerrainVtxPos[dwVtxNumber[1]].z));
			}

			// 왼쪽 아래
			dwVtxNumber[0] = dwIndex + VTXCNTX;
			dwVtxNumber[1] = dwIndex + 1;
			dwVtxNumber[2] = dwIndex;

			if (D3DXIntersectTri(&pTerrainVtxPos[dwVtxNumber[2]],
				&pTerrainVtxPos[dwVtxNumber[1]],
				&pTerrainVtxPos[dwVtxNumber[0]],
				&vRayPos, &vRayDir,
				&fU, &fV, &fDist))
			{
				return _vec3(pTerrainVtxPos[dwVtxNumber[2]].x + fU * (pTerrainVtxPos[dwVtxNumber[1]].x - pTerrainVtxPos[dwVtxNumber[2]].x),
					0.f,
					pTerrainVtxPos[dwVtxNumber[2]].z + fV * (pTerrainVtxPos[dwVtxNumber[0]].z - pTerrainVtxPos[dwVtxNumber[2]].z));
			}
		}
	}

	return _vec3(0.f, 0.f, 0.f);
}

CCalculator* CCalculator::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCalculator* pCalculator = new CCalculator(pGraphicDev);

	if (FAILED(pCalculator->Ready_Calculator()))
	{
		Safe_Release(pCalculator);
		MSG_BOX("Calculator Create Failed");
		return nullptr;
	}

	return pCalculator;
}

CComponent* CCalculator::Clone()
{
	return new CCalculator(*this);
}

void CCalculator::Free()
{
	Safe_Release(m_pGraphicDev);
}
