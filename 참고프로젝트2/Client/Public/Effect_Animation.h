#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CEffect_Animation : public CBase
{
private:
	CEffect_Animation();
	CEffect_Animation(const CEffect_Animation& Prototype);
	virtual ~CEffect_Animation() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg = nullptr);

public:
	void		Add_KeyFrame(_uint KeyFrameNumber, EFFECT_KEYFRAME NewKeyFrameData);
	void		Delete_KeyFrame(_uint KeyFrameNumber);
	_bool	Find_KeyFrame(_uint KeyFrameNumber);
	EFFECT_KEYFRAME Get_KeyFrame(_uint KeyFrameNumber);
	EFFECT_KEYFRAME Get_Near_Front_KeyFrame(_uint KeyFrameNumber);
	EFFECT_KEYFRAME Play_Animation(_float CurAnimPos, _bool bIsLoop);
	_float3 Lerp(const _float3& start, const _float3& end, _float factor);

public:
	map<_uint, EFFECT_KEYFRAME>	m_EffectKeyFrames;

private:
	class CGameInstance* m_pGameInstance = { nullptr };



public:
	static CEffect_Animation* Create();
	CEffect_Animation* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END;