#pragma once

#include "Base.h"
#include "Utility/LogicDeviceBasic.h"

#define LIGHT_DEBUG 0

BEGIN(Engine)

class ENGINE_DLL CLight  final : public CBase
{
private:
	CLight();
	virtual ~CLight() = default;

public:
	HRESULT Initialize(const LIGHT_DESC& LightDesc);
	void Tick(_cref_time fTimeDelta);
	HRESULT Render(shared_ptr<class CShader> pShader, shared_ptr<class CVIBuffer_Rect> pVIBuffer);
#ifdef _DEBUG
	HRESULT Render_Debug();
#endif // _DEBUG


public:
	// 라이트의 AABB를 계산해줄 함수
	void Invalidate_Light();
	_bool Intersects(const BoundingFrustum& Frustum);


public:
	GETSET_EX2(_bool, m_bTurnOn, TurnOnLight, GET, SET)
	GETSET_EX2(_float, m_fRangeLinearDecrease, RangeLinearDecrease, GET, SET)
	GETSET_EX2(_float, m_fRangeQuadDecrease, RangeQuadDecrease, GET, SET)

	// 라이트를 동적으로 활용하기 위한 캡슐화
	void Set_LightPosition(_float4 vPos);
	const _float4 Get_LightPosition();

	void Set_LightRange(_float fRange);
	const _float Get_LightRange();
	
	void Set_LightDamping(_float fQuad = 0.f, _float fLinear = 1.f, _float fConst = 1.f);
	void Set_LightVolumeQuadDamping(_float fVolumeQuadDamping = 0.f);

	void Set_LightDesc(LIGHT_DESC Desc);
	const LIGHT_DESC& Get_LightDesc();

	_bool Get_Dead() { return m_bDead; }
	void  Set_Dead() { m_bDeadReady = true; }
	void  Dead_Ready(_cref_time fTimeDelta);

	const _bool& Is_UseLifeTime() const { return m_bUseLifeTime; }
	void	Set_LifeTime(_float fLifeTime) { m_fLifeTime = fLifeTime; m_bUseLifeTime = true; }
	_bool	Tick_LifeTime(_cref_time fTimeDelta) { return m_fLifeTime.Increase(fTimeDelta); }

private:
	LIGHT_DESC			m_LightDesc;						// 빛의 정보를 저장
	_bool				m_bTurnOn = { true };				// 빛의 켜짐 유무 체크
	BoundingBox			m_AABB;								// 빛의 영역정보를 저장, BroadPhase에 사용됨
	_bool				m_bNeedInvalidate = { false };		// 빛 정보의 변동을 체크하는 함수
	_bool				m_bIsEnabled = { false };

	_bool				m_bDeadReady = { false };
	_bool				m_bDead = { false };

	_bool				m_bUseLifeTime = { false };
	FGauge				m_fLifeTime = { 0.f };

	_float				m_fRangeLinearDecrease = { 1.f };
	_float				m_fRangeQuadDecrease = { 0.f };

public:
	void Update_LightOwner();

	void Set_Owner(weak_ptr<CGameObject> _pOwner);
	weak_ptr<CGameObject> Get_Owner();


private:
	_bool				m_bIsGetOwner = { false };
	weak_ptr<CGameObject>	m_pOwner;
	
#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*		m_pBatch = { nullptr };
	BasicEffect*								m_pEffect = { nullptr };
	ComPtr<ID3D11InputLayout>					m_pInputLayout = { nullptr };
#endif // _DEBUG


public:
	static shared_ptr<CLight> Create(const LIGHT_DESC& LightDesc);
	virtual void Free() override;
};

END