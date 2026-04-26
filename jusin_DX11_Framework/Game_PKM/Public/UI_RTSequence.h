#pragma once
#include "Base.h"
#include "Game_PKM_Defines.h"

NS_BEGIN(Engine)
class CGameInstance;
class CUIObject;
class CUISequence;
NS_END

NS_BEGIN(Game_PKM)

class CUI_RTSequence final : public CBase
{
private:
	CUI_RTSequence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_RTSequence() = default;

public:
	HRESULT Initialize(const _string& strPath, _uint iLevel, Engine::WNameID strLayerTag);
	void Update(_float fTimeDelta);
	void Play();
	void Stop();

	CUIObject* Find_Widget(const _string& strId) const;

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

	_uint m_iLevel = { 0 };
	WNameID m_strLayerTag = { };
	_bool m_bPlaying = { false };

	CUISequence* m_pSequence = { nullptr };

	// weak ref : 실제 소유는 Layer
	vector<CUIObject*> m_vWidgets;
	unordered_map<_string, Engine::CUIObject*> m_mapById;

public:
	static CUI_RTSequence* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _string& strPath, _uint iLevel, WNameID strLayerTag);

private:
	virtual void Free() override;
};

NS_END