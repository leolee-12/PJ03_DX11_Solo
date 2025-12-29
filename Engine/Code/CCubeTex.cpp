#include "CCubeTex.h"	// (P)

CCubeTex::CCubeTex()
{
}

CCubeTex::CCubeTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CCubeTex::CCubeTex(const CCubeTex& rhs)
	: CVIBuffer(rhs)
{
}

CCubeTex::~CCubeTex()
{
}

HRESULT CCubeTex::Ready_Buffer()
{
	m_dwVtxSize = sizeof(VTXCUBE);	// 큐브텍스처용 VTXCUBE 사용
	m_dwVtxCnt = 8;
	m_dwTriCnt = 12;
	m_dwFVF = FVF_CUBE;				// 버텍스에 따라 FVF도 바꾸어준다
	// - FVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);
	// -> TexUV 좌표 값을 FLOAT형 3개(_vec3)로 표현하겠다는 매크로
	// -> 괄호안의 숫자 0 : 버텍스에 TexUV값이 여러 개가 올 수 있음. 그 중 0번째 값을 지정하겠다는 의미

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXCUBE* pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	// 좌상 -> 우상 -> 우하 -> 좌하 순으로 버텍스 설정
	// - 어디서부터든 상관없으나 통일성은 필요, 인덱스 설정에만 유의)
	// - _vec3 TexUV는 버텍스의 위치와 동일하게 설정
	// -> 큐브 텍스처의 경우, 카메라를 (0, 0, 0)에 두었을 때 특정 방향으로 바라보았을 때의 텍스처를 TexUV값으로 가짐

	pVertex[0].vPosition = pVertex[0].vTexUV = { -1.f, 1.f, -1.f };
	pVertex[1].vPosition = pVertex[1].vTexUV = { 1.f, 1.f, -1.f };
	pVertex[2].vPosition = pVertex[2].vTexUV = { 1.f, -1.f, -1.f };
	pVertex[3].vPosition = pVertex[3].vTexUV = { -1.f, -1.f, -1.f };

	pVertex[4].vPosition = pVertex[4].vTexUV = { -1.f, 1.f, 1.f };
	pVertex[5].vPosition = pVertex[5].vTexUV = { 1.f, 1.f, 1.f };
	pVertex[6].vPosition = pVertex[6].vTexUV = { 1.f, -1.f, 1.f };
	pVertex[7].vPosition = pVertex[7].vTexUV = { -1.f, -1.f, 1.f };

	m_pVB->Unlock();

	INDEX32* pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	//x+
	pIndex[0]._0 = 1; pIndex[0]._1 = 5; pIndex[0]._2 = 6;
	pIndex[1]._0 = 1; pIndex[1]._1 = 6; pIndex[1]._2 = 2;

	//x-
	pIndex[2]._0 = 4; pIndex[2]._1 = 0; pIndex[2]._2 = 3;
	pIndex[3]._0 = 4; pIndex[3]._1 = 3; pIndex[3]._2 = 7;

	//y+
	pIndex[4]._0 = 4; pIndex[4]._1 = 5; pIndex[4]._2 = 1;
	pIndex[5]._0 = 4; pIndex[5]._1 = 1; pIndex[5]._2 = 0;

	//y-
	pIndex[6]._0 = 3; pIndex[6]._1 = 2; pIndex[6]._2 = 6;
	pIndex[7]._0 = 3; pIndex[7]._1 = 6; pIndex[7]._2 = 7;

	//z+
	pIndex[8]._0 = 7; pIndex[8]._1 = 6; pIndex[8]._2 = 5;
	pIndex[9]._0 = 7; pIndex[9]._1 = 5; pIndex[9]._2 = 4;

	//z-
	pIndex[10]._0 = 0; pIndex[10]._1 = 1; pIndex[10]._2 = 2;
	pIndex[11]._0 = 0; pIndex[11]._1 = 2; pIndex[11]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

void CCubeTex::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CCubeTex* CCubeTex::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCubeTex* pCubeTex = new CCubeTex(pGraphicDev);

	if (FAILED(pCubeTex->Ready_Buffer()))
	{
		Safe_Release(pCubeTex);
		MSG_BOX("pCubeTex Create Failed");
		return nullptr;
	}

	return pCubeTex;
}

CComponent* CCubeTex::Clone()
{
	return new CCubeTex(*this);
}

void CCubeTex::Free()
{
	CVIBuffer::Free();
}
