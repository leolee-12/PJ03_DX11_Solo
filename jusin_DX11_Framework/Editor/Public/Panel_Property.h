#pragma once
#include "Panel_Base.h"

NS_BEGIN(Engine)
class CTransform;
class CGameObject;
NS_END

NS_BEGIN(Editor)

class CPanel_Property final : public CPanel_Base
{
protected:
	CPanel_Property();
	virtual ~CPanel_Property() = default;

public:
	HRESULT Initialize() override;
	void Update(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	using PropertyDrawFn = function<void(CGameObject*)>;
	unordered_map<_string, PropertyDrawFn> m_DrawerTable;
	_char m_szNameBuffer[128] = {};
	CGameObject* m_pLastSelected = { nullptr };

	void Draw_Transform(CTransform* pTransformCom);
	void Draw_TypeProps(CGameObject* pObj);
	void Register_Drawers();

public:
	static CPanel_Property* Create();

private:
	virtual void Free() override;
};

NS_END