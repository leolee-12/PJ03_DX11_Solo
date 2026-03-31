#pragma once
#include "Panel_Base.h"

NS_BEGIN(Editor)

class CPanel_PlaceBrowser final : public CPanel_Base
{
public:
	struct CATALOG_ITEM
	{
		WNameID protoTag;
		_string strDisplayName;
		_string strCategory;
	};

protected:
	CPanel_PlaceBrowser();
	virtual ~CPanel_PlaceBrowser() = default;

public:
	HRESULT Initialize() override;
	void Update(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	vector<CATALOG_ITEM> m_AllItems;
	unordered_map<_string, vector<CATALOG_ITEM*>> m_ByCategory;
	_char m_szFilter[128] = {};
	_uint m_iProtoLevel = {};
	_uint m_iLayerLevel = {};
	WNameID m_LayerTag = {};

private:
	void Register_Items();
	void Draw_Category(const _string& strCat);
	void Place_Object(const CATALOG_ITEM tItem);

public:
	static CPanel_PlaceBrowser* Create();

private:
	virtual void Free() override;
};

NS_END