#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CLobby_Goku final : public CGameObject
{
private:
	CLobby_Goku(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLobby_Goku(const CLobby_Goku& Prototype);
	virtual ~CLobby_Goku() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void RotateTowardsTarget(const _float3& vTargetDir, _float fTimeDelta);
	void MoveForward(_float fTimeDelta);
	void Entry_Level();

	void Set_CharacterInfo();

	//해당 레이어 오브젝트와의 거리 체크
	_float ObjectDistance(_wstring strLayerTag ,_uint iLayerIndex = 0);

	//프리저와 상호작용
	void Talk_Frieza(_float fEnableDistance);
	void Talk_Staff(_float fEnableDistance);


	_bool m_bOnMessageBox[3] = {FALSE ,FALSE ,FALSE};

	//걸었을 때 먼지 이펙트
	void CreateRunDustEffect(_bool bOnInput, _float fCreateDuration,_float fTimeDelta);

	_float CreateDustTimer = { 0.f };

	// 애니메이션 상태 관리
	_bool m_bDashTriggered = false; // 대쉬 애니메이션이 트리거되었는지 여부

	// 이전 키 상태 저장
	_bool m_bPrevKeyUp = false;
	_bool m_bPrevKeyDown = false;
	_bool m_bPrevKeyLeft = false;
	_bool m_bPrevKeyRight = false;

	// 발소리 타이머
	_float m_fFootstepTimer = 0.f;
	// 발소리 재생 간격 (초)
	_float m_fFootstepInterval = 0.3f; // 예: 0.5초마다 발소리 재생

private:
	//그림자
	_bool					m_isShadow = { true };
	_uint					m_iPassIndex = { 0 };

	_bool					m_bNextLevelText = { FALSE };

public:
	static CLobby_Goku* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END