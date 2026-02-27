#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_Input abstract : public CUIObject
{
public:
	typedef struct :public CUIObject::UI_DESC
	{
		DirectionInput eDirInput = {};
		ButtonInput		eActionInput = {  };

		_bool  isGrab = {};
	}UI_INPUT_DESC;

protected:
	CUI_Input(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Input(const CUI_Input& Prototype);
	virtual ~CUI_Input() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

protected:
	virtual HRESULT Bind_ShaderResources();
	virtual HRESULT Ready_Components();

protected:
	_uint CreateList(UI_LRPOS ePos);
	_uint CreateList2(UI_LRPOS ePos);

	_bool m_bInitCommand = { FALSE };
	_bool m_bInitCommandAction = { FALSE };
	DirectionInput m_eDir= { MOVEKEY_NEUTRAL };
	ButtonInput m_eAction = { ATTACK_NONE };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

