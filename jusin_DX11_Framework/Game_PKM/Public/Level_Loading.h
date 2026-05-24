#pragma once
#include "Game_PKM_Defines.h"
#include "Game_LevelEntry.h"

#include "Level.h"

NS_BEGIN(Engine)
class CUISequence;
NS_END

/* -------------------------------------------------- */
// 로딩 레벨
// - 로딩 화면에 필요한 객체(배경, 로딩바, 로딩 텍스트 등)들을 생성
// - 로딩 레벨을 갱신하여 화면에 띄움
// - 다음 레벨을 위한 자원 준비
/* -------------------------------------------------- */

NS_BEGIN(Game_PKM)

class CLevel_Loading : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID, const LEVEL_ENTRY_DESC* pEntryDesc = nullptr);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	LEVEL_ENTRY_DESC m_tEntryDesc = {};
	class CLoader* m_pLoader = { nullptr };
	CUISequence* m_pLoadingUI = { nullptr };

private:
	HRESULT Ready_Layer_UI(WNameID strLayerTag);

public:
	static CLevel_Loading*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID, const LEVEL_ENTRY_DESC* pEntryDesc = nullptr);

protected:
	virtual void Free() override;
};

NS_END