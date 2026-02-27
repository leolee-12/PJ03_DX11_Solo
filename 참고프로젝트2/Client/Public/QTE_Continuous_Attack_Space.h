#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CQTE_Continuous_Attack_Space final : public CGameObject
{
public:
	struct CONTINUOUS_ATTACK_DESC
	{
		_float	fSizeX{}, fSizeY{}, fX{}, fY{};
	};

private:
	CQTE_Continuous_Attack_Space(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQTE_Continuous_Attack_Space(const CQTE_Continuous_Attack_Space& Prototype);
	virtual ~CQTE_Continuous_Attack_Space() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	void Process_Command();
public:
	_float					m_fSizeX{}, m_fSizeY{}, m_fX{}, m_fY{};
	_float4x4				m_ViewMatrix{}, m_ProjMatrix{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void Update_Animation(_float fTimeDelta);

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	// 위치 관련 변수
	_float m_fDefaultY = {  };  // 기본 위치
	_float m_fTargetY = {  };    // 타겟 위치

	// 애니메이션 제어 변수
	_float m_fMoveDownTime = 0.1f;    // 내려가는 데 걸리는 시간
	_float m_fMoveUpTime = 0.1f;      // 올라가는 데 걸리는 시간
	_float m_fCurrentTime = 0.0f;     // 현재 애니메이션 진행 시간

	_bool m_bIsMoving = false;        // 애니메이션 진행 여부
	_bool m_bIsMovingDown = false;    // 내려가는 중인지 여부

public:
	static CQTE_Continuous_Attack_Space* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END