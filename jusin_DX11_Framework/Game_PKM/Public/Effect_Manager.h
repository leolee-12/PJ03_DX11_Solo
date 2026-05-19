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

	/* M7a: ATTACH_INFO 없이 vSpawnPos만. 반환은 borrowed (Layer ref 보유). */
	CEffect* Spawn(const _string& strID,
		const _float3& vSpawnPos,
		_uint iLevel,
		WNameID strLayerTag,
		const CEffect::EFFECT_DESC::ATTACH_INFO& tAttach = {});

private:
	unordered_map<_string, EFFECT_DEFINITION> m_Definitions;
	_bool m_bInitialized = false;

private:
	virtual void Free() override;
};

NS_END