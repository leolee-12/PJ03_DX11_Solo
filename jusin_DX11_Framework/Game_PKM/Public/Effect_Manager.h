#pragma once
#include "Base.h"
#include "Game_PKM_Defines.h"
#include "Effect_Defines.h"
#include "Effect.h"

NS_BEGIN(Game_PKM)
class CEffect;

class CEffect_Manager final : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager)

private:
	CEffect_Manager();
	virtual ~CEffect_Manager() = default;

public:
	HRESULT Initialize();

	HRESULT Register_Definition(const EFFECT_DEFINITION& def);
	const EFFECT_DEFINITION* Find_Definition(const _string& strID) const;
	HRESULT Load_Definitions(const _char* pFolderPath);

	CEffect* PlayAt(const _string& strID,
		const _float3& vWorldPos,
		_uint iLevel = INVALID_INDEX,
		WNameID strLayerTag = LAYER_EFFECT);

	CEffect* PlayAttached(const _string& strID,
		const CEffect::EFFECT_DESC::ATTACH_INFO& tAttach,
		_uint iLevel = INVALID_INDEX,
		WNameID strLayerTag = LAYER_EFFECT);

private:
	unordered_map<_string, EFFECT_DEFINITION> m_Definitions;
	_bool m_bInitialized = false;

private:
	CEffect* Spawn(const _string& strID,
		const _float3& vSpawnPos,
		_uint iLevel,
		WNameID strLayerTag,
		const CEffect::EFFECT_DESC::ATTACH_INFO& tAttach);

private:
	virtual void Free() override;
};

NS_END