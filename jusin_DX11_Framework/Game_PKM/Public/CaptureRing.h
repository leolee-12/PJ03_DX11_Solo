#pragma once
#include "PartObject.h"
#include "Game_PKM_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
NS_END

NS_BEGIN(Game_PKM)

class CCaptureRing : public CPartObject
{
	struct MONSTER_BALL_DESC : public CPartObject::PARTOBJECT_DESC
	{
		_float3 vTargetPos = { 0.f, 0.f, 0.f };       // 카메라 vAt = CaptureTarget 위치
		_float  fFlightDuration = 1.0f;                    // 비행 시간(초)
		_float  fArcHeight = 2.0f;                    // 포물선 최고점 추가 높이
		_float  fImpactDuration = 0.5f;                    // IMPACT 정지 시간(초)
	};

private:
	CCaptureRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCaptureRing(const CCaptureRing& Prototype);
	virtual ~CCaptureRing() = default;

public:
	virtual _string Get_TypeName() const override { return "CaptureRing"; }

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Priority_Update(_float fTimeDelta) override;
	virtual void    Update(_float fTimeDelta) override;
	virtual void    Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

public:
	static CCaptureRing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END