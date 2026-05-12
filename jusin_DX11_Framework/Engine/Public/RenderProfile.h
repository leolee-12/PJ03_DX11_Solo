#pragma once
#include "Engine_Defines.h"
#include "RenderRule.h"

NS_BEGIN(Engine)

class CModel;
class CShader;

class ENGINE_DLL CRenderProfile
{
public:
	struct MATERIAL_SLOT
	{
		MATERIAL_TYPE eType = { MATERIAL_TYPE::DIFFUSE };
		const _char* pConstName = { nullptr };
	};

public:
	HRESULT Build(CModel* pModel, const CRenderRule* pRule);

	HRESULT Bind_AndDraw(
		CShader* pShader,
		const vector<MATERIAL_SLOT>& Slots,
		const _char* pBoneMatricesConstName = nullptr);

	void Set_Pass(_uint iMatIdx, _uint iPass);
	void Set_Variant(_uint iMatIdx, MATERIAL_TYPE eType, _uint iSlot);

	const RENDER_TABLE& Get_Table() const { return m_Table; }
	CModel* Get_Model() const { return m_pModel; }

private:
	CModel* m_pModel = { nullptr }; // weak
	RENDER_TABLE m_Table;

private:
	_bool Has_Conflict(_uint iMatIdx, const CRenderRule::RESOLVE_RESULT& Result) const;
	void Apply_Result(_uint iMatIdx, const CRenderRule::RESOLVE_RESULT& Result);
};

NS_END