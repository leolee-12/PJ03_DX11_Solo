#pragma once

#include "UIObject.h"
#include <queue>

BEGIN(Engine)

class CVIBuffer_Trail_Rect;

END

BEGIN(Client)

class CUI_Loading_FlyEff final :public CUIObject
{
public:
	typedef struct : public  UI_DESC
	{
		CUI_Manager::ThreadPool_For_Loading eTheadID = { CUI_Manager::THREAD_END };
	}UI_FLYEFF_DESC;

private:
	CUI_Loading_FlyEff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Loading_FlyEff(const CUI_Loading_FlyEff& Prototype);
	virtual ~CUI_Loading_FlyEff() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

public:
	CUI_Manager::ThreadPool_For_Loading Get_ThreadID() { return ThreadID; }
	_bool Get_AnimEnd() { return m_bCreateBall; }

private:
	virtual HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources();

private:
	_bool Go_Target(_vector  fTargetPos , _float fTimeDelta);
	void Animation(_vector  fTargetPos, _float fTimeDelta);

	void AnimSpeed(_float fTimeDelta);

private:
	_bool m_bEndAnim = { FALSE };
	_bool m_bCreateBall = { FALSE };

	_float m_fAnimFream = { 0.f };
	_float m_fAnimOffset = { 0.f };

	CTransform* m_pHoleTransform = { nullptr };

	queue<_vector> QueueAnim = {};
	_float m_fScaleOffset = {1.f};

	_uint m_iRandomMove = { 0 };
	_float m_fSpeedVaule = { 0.f };

	CUI_Manager::ThreadPool_For_Loading ThreadID = {};

public:
	static CUI_Loading_FlyEff* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END