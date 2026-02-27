#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_Lobby_Text final :public CUIObject
{
public:
	typedef struct
	{
		_wstring strText = {};
		CUI_Define::NPC_ID   eNpcID = { CUI_Define::ID_END };
	}UI_TEXT;

private:
	CUI_Lobby_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Lobby_Text(const CUI_Lobby_Text& Prototype);
	virtual ~CUI_Lobby_Text() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

public:
	_bool Get_Finish() { return m_bFinishText; }

private:
	virtual HRESULT Ready_Components();

public:
	void Add_Text(queue<UI_TEXT> TextInfo);

private:
	void TextOutput(_float fOutputSpeed, _float fTimeDelta);
	void SimpleAnim(_float fTimeDelta);

private:
	_bool m_bFinishText = { FALSE };

	_float m_fFontDration = { 0.f };
	_uint m_iTextIndex = { 0 };

	queue<UI_TEXT> m_vTextArray = {};
	_wstring m_strOutputText = {};
	
	_float m_fOffsetPosY = { 0.f };
public:
	static CUI_Lobby_Text* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END