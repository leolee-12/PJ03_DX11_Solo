#pragma once
#include "Game_PKM_Defines.h"
#include "Body.h"

NS_BEGIN(Game_PKM)

class CBody_Hero final : public CBody
{
public:
	using BODY_HERO_DESC = CBody::BODY_DESC;

	enum MATERIAL_NAME { BAG, BOTTOMS, CAP, R_EYE, L_EYE, SKIN, HAIR, SHOES, TOPS, END };

private:
	CBody_Hero(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Hero(const CBody_Hero& Prototype);
	virtual ~CBody_Hero() = default;

public:
	void Set_Variant(_uint iMatIdx, MATERIAL_TYPE eType, _uint iMatNum) { m_RenderTable.variants[iMatIdx][ETOUI(eType)] = iMatNum; }
	void Set_Pass(_uint iMatIdx, _uint iPassIdx) { m_RenderTable.passes[iMatIdx] = iPassIdx; }

	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	_uint m_iDummy = {};
	RENDER_TABLE m_RenderTable;

private:
	void Ready_DefaultVariant();

public:
	static CBody_Hero* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free();
};

NS_END