#pragma once
#include "Body_BattleBase.h"

NS_BEGIN(Game_PKM)

class CBody_BattleHeroLGPE final : public CBody_BattleBase
{
public:
	enum MATERIAL_NAME { BAG, BOTTOMS, CAP, R_EYE, L_EYE, SKIN, HAIR, SHOES, TOPS, END };

private:
	CBody_BattleHeroLGPE(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_BattleHeroLGPE(const CBody_BattleHeroLGPE& Prototype);
	virtual ~CBody_BattleHeroLGPE() = default;

public:
	void Set_Variant(unsigned int iMatIdx, MATERIAL_TYPE eType, unsigned int iMatNum);
	void Set_Pass(unsigned int iMatIdx, unsigned int iPassIdx);

	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

private:
	RENDER_TABLE m_RenderTable;

private:
	void Ready_DefaultVariant();

public:
	static CBody_BattleHeroLGPE* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END