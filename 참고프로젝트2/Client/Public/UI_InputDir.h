#pragma once

#include "UI_Input.h"

BEGIN(Client)

class CUI_InputDir final :public CUI_Input
{
private:
	CUI_InputDir(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_InputDir(const CUI_InputDir& Prototype);
	virtual ~CUI_InputDir() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

private:
	virtual HRESULT Ready_Components();

private:
	_float RotaionValue(_float2 vPrevPos ,_float2 vCurrPos);
	_float ScaleValue(_float2 vPrevPos, _float2 vCurrPos);
	_vector CreatePostion(_float2 vPrevPos, _float2 vCurrPos);

	void MovePos(DirectionInput eInput, _float& fPosX, _float& fPosY);
	void LineEffectCreate();

private:
	_bool m_bCheck = { FALSE };

	DirectionInput ePrevDirInput = {};
	_float2 m_vPos = {};
	_bool m_bOnBtn = { FALSE };
	
	_uint m_iTextureIndex = { 6 };
	_float m_fOnTimer = { 0.f };

	_float m_fOffsetScaled = {0.f};

public:
	static CUI_InputDir* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END