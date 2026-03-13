#include "pch.h"	//	(T)
#include "CEffect.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"

CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_fFrame(0.f)
{
}

CEffect::CEffect(const CGameObject& rhs)
	: CGameObject(rhs), m_fFrame(0.f)
{
}

CEffect::~CEffect()
{
}

HRESULT CEffect::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	_float x = (rand() % 4) * 1.f + (rand() % 10) * 0.1f + (rand() % 10) * 0.01f;
	_float z = (rand() % 4) * 1.f + (rand() % 10) * 0.1f + (rand() % 10) * 0.01f;


	m_pTransformCom->Set_Pos(x, 0.f, z);
	// * 난수 사용 시 한 프레임 내에서 시드 갱신 + 난수 사용을 동시에 여러 번 수행하면 안됨
	// - 적은 연산으로는 프레임 내에서 시간 경과 X -> 항상 같은 시드가 입력
	// - 시드가 갱신되면 그 시드의 첫 번째 난수 사용 -> 매 프레임 갱신해버리면 계속 첫 번째 난수만 사용
	// -> 다른 곳에서 최초 1회 시드 적용한 뒤 사용하기

	m_fFrame = 0.f;

	return S_OK;
}

_int CEffect::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	m_fFrame += 90.f * fTimeDelta;

	if (90.f < m_fFrame)
		m_fFrame = 0.f - _float(rand() % 180);


	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	// - 알파블렌딩 시 z버퍼로 인한 문제 (물체가 겹칠 시 z버퍼는 작은 z값만 저장)
	// - 알파테스팅 시 빗살무늬로 텍스처가 사라지는 z Fighting을 해결할 수 있으나, 블렌딩에 비해 표현이 다소 아쉬움
	// -> 알파 소팅 !
	// --> z 버퍼 기준 정렬을 사용하지 않음
	// --> 카메라 기준 거리 정렬을 사용자가 직접 수행 (연산량 많음)

	return iExit;
}

void CEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	// 빌보드 적용
	// - 후면 추려내기 및 2D 이미지여서 후면, 측면에서는 보이지 않는 문제
	// - 2D 텍스처를 렌더하는 오브젝트들이 항상 카메라의 근평면을 바라보게 하자 = 빌보드
	// -> 오브젝트 Update : 카메라와의 거리 구하기, LateUpdate : 월드행렬을 재구성
	_matrix	matWorld, matView, matBill;

	matWorld = *m_pTransformCom->Get_World();

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matBill);

	// y축 회전 관련 요소 : _11, _13, _31, _33
	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;
	// - 뷰스페이스변환행렬에는 이미 회전 변환이 적용되어 있는 상태
	// -> y축 회전 관련 값만 빌보드 행렬에 대입해주기

	D3DXMatrixInverse(&matBill, 0, &matBill);
	// - y축 회전 행렬을 반대로 적용하기 위해 역행렬로 만듬

	matWorld = matBill * matWorld;
	// - 순서에 유의
	// - 텍스처 뿐만 아니라, 메쉬에도 적용 가능
	
	// * 수업 코드로는 문제없으나, 메쉬에 빌보드를 적용할 때는 스케일 고려가 필요할 수 있음
	// - 수업 코드에서의 월드 행렬 : I(스케일값 default) * 회전 * 이동
	// - 빌보드 적용된 월드 행렬 : 회전역행렬 * I * 회전 * 이동
	// - 메쉬의 경우, 스케일을 매우 줄여서 사용하는 경우가 많음
	// -> 빌보드 적용 시 회전역행렬에 의해 크기값이 달라질 수 있으니 유의

	m_pTransformCom->Set_World(&matWorld);

	_vec3		vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	Compute_ViewZ(&vPos);	// 카메라와의 z 거리를 계산

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CEffect::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);
	// Update에서 timedelta로 m_fFrame값을 누적시키고, 이를 정수값만 취하는 식으로 스프라이트를 넘김
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CEffect::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcCol
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_EffectTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });


	return S_OK;
}



CEffect* CEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffect* pEffect = new CEffect(pGraphicDev);

	if (FAILED(pEffect->Ready_GameObject()))
	{
		Safe_Release(pEffect);
		MSG_BOX("pEffect Create Failed");
		return nullptr;
	}

	return pEffect;
}

void CEffect::Free()
{
	CGameObject::Free();
}
