#pragma once

#include "UIObject.h"

BEGIN(Client)

class CUI_CharaSelectIcon final :public CUIObject
{
private:
	CUI_CharaSelectIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CharaSelectIcon(const CUI_CharaSelectIcon& Prototype);
	virtual ~CUI_CharaSelectIcon() = default;

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
	void SelectIcon(_float fPosX , _float fPosY);
	void InputEvent(_uint iKey , CUI_Define::PLAYER_ID ePlayerID);
	void CreateChoiceMark(CUI_Define::PLAYER_ID ePlayerID);
	void CharacterCreateDesc(CUI_Define::PLAYER_ID ePlayerID);
	void CreateSelectModel(CUI_Define::PLAYER_ID ePlayerID);
	void CreateCharaImage();
	void CreateSelectLine();
	void ArrowToPlayerID(CUI_Define::PLAYER_ID eID);

	_bool OverlapCheck(CUI_Define::PLAYER_ID ePlayerID);

private:
	_uint m_iTexIndex = { 0 };
	CTransform* m_pArrowTransform = { nullptr };
	vector<CUI_Define::PLAYER_ID> m_ePlayerID = {};
	_bool m_bOverClear = { FALSE };
	_bool m_bOnCursor = { FALSE };

public:
	static CUI_CharaSelectIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END