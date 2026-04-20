#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)

class CUIContainer abstract : public CUIObject
{
public:
	struct UICONTAINER_DESC : public CUIObject::UIOBJECT_DESC
	{
		UILAYOUT_DESC tLayoutDesc{};
	};

protected:
	CUIContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIContainer(const CUIContainer& Prototype);
	virtual ~CUIContainer() = default;

public:
	virtual _string Get_TypeName() const override { return "UIContainer"; }
	virtual UI_TYPE Get_UIType() const override { return UI_TYPE::CONTAINER; }
	void Set_Layout(const UILAYOUT_DESC& tLayoutDesc);
	const vector<CUIObject*>& Get_Children() const { return m_Children; }


	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Refresh_Layout() override;
	void Add_Child(CUIObject* pChild);
	void Remove_Child(CUIObject* pChild);

protected:
	vector<CUIObject*> m_Children;
	UILAYOUT_DESC m_tLayoutDesc = {};

protected:
	void Arrange_Children();
	void Arrange_Overlay();
	void Arrange_Horizontal();
	void Arrange_Vertical();

public:
	static CUIContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

protected:
	virtual void Free();
};

NS_END