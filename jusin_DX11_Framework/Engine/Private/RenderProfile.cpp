#include "RenderProfile.h"

#include "Model.h"
#include "Shader.h"
#include "RenderRule.h"
#include "Profiler_Manager.h"

HRESULT CRenderProfile::Build(CModel* pModel, const CRenderRule* pRule)
{
	if (nullptr == pModel)
		return E_FAIL;

	m_pModel = pModel;
	m_Table.Ready_RenderTable(pModel->Get_NumMaterials());

	vector<_bool> SeenMaterials;
	SeenMaterials.assign(pModel->Get_NumMaterials(), false);

	const size_t iNumMeshes = pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		const _uint iMatIdx = pModel->Get_MeshMaterialIndex(i);
		if (iMatIdx >= m_Table.passes.size())
			return E_FAIL;

		if (nullptr == pRule)
			continue;

		CRenderRule::RESOLVE_RESULT Result{};
		if (false == pRule->Resolve(pModel->Get_MeshName(i), iMatIdx, Result))
			continue;

		if (false == SeenMaterials[iMatIdx])
		{
			Apply_Result(iMatIdx, Result);
			SeenMaterials[iMatIdx] = true;
		}
		else if (true == Has_Conflict(iMatIdx, Result))
		{
			OutputDebugStringA("[RenderProfile] Material rule conflict ignored.\n");
		}
	}

	return S_OK;
}

HRESULT CRenderProfile::Bind_AndDraw(
	CShader* pShader,
	const vector<MATERIAL_SLOT>& Slots,
	const _char* pBoneMatricesConstName,
	const vector<_bool>* pVisibleMask,
	_uint* pDrawnMeshCount)
{
	PROFILE_CPU_SCOPE(L"CPU_RenderProfile_BindAndDraw");

	if (nullptr == m_pModel || nullptr == pShader)
		return E_FAIL;

	const size_t iNumMeshes = m_pModel->Get_NumMeshes();

	if (nullptr != pDrawnMeshCount)
		*pDrawnMeshCount = 0;

	const _bool bUseVisibleMask =
		nullptr != pVisibleMask && pVisibleMask->size() == iNumMeshes;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (true == bUseVisibleMask && false == (*pVisibleMask)[i])
			continue;

		const _uint iMatIdx = m_pModel->Get_MeshMaterialIndex(i);
		if (iMatIdx >= m_Table.passes.size() || iMatIdx >= m_Table.variants.size())
			return E_FAIL;

		for (const auto& Slot : Slots)
		{
			if (nullptr == Slot.pConstName)
				continue;

			const _uint iType = ETOUI(Slot.eType);
			if (iType >= ETOUI(MATERIAL_TYPE::END))
				return E_FAIL;

			const _uint iTextureIndex = (Slot.iTextureIndex == -1 
				? m_Table.variants[iMatIdx][iType] : Slot.iTextureIndex);

			if (FAILED(m_pModel->Bind_Material(
				pShader,
				Slot.pConstName,
				i,
				Slot.eType,
				iTextureIndex)))
				return E_FAIL;
		}

		if (nullptr != pBoneMatricesConstName)
		{
			if (FAILED(m_pModel->Bind_BoneMatrices(pShader, pBoneMatricesConstName, i)))
				return E_FAIL;
		}

		const _uint iPass = m_Table.passes[iMatIdx];
		if (iPass >= pShader->Get_NumPasses())
		{
			OutputDebugStringA("[RenderProfile] Invalid shader pass index.\n");
			return E_FAIL;
		}

		if (true == m_Table.useLayerColors[iMatIdx])
		{
			if (FAILED(pShader->Bind_RawValue("g_vLymColorR", &m_Table.layerColors[iMatIdx][0], sizeof(_float4))))
				return E_FAIL;

			if (FAILED(pShader->Bind_RawValue("g_vLymColorG", &m_Table.layerColors[iMatIdx][1], sizeof(_float4))))
				return E_FAIL;

			if (FAILED(pShader->Bind_RawValue("g_vLymColorB", &m_Table.layerColors[iMatIdx][2],	sizeof(_float4))))
				return E_FAIL;

			if (FAILED(pShader->Bind_RawValue("g_vLymColorA", &m_Table.layerColors[iMatIdx][3],	sizeof(_float4))))
				return E_FAIL;
		}

		if (FAILED(pShader->Begin(iPass)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(i)))
			return E_FAIL;

		if (nullptr != pDrawnMeshCount)
			++(*pDrawnMeshCount);
	}

	return S_OK;
}

void CRenderProfile::Set_Pass(_uint iMatIdx, _uint iPass)
{
	if (iMatIdx >= m_Table.passes.size())
		return;

	m_Table.passes[iMatIdx] = iPass;
}

void CRenderProfile::Set_Variant(_uint iMatIdx, MATERIAL_TYPE eType, _uint iSlot)
{
	if (iMatIdx >= m_Table.variants.size())
		return;

	const _uint iType = ETOUI(eType);
	if (iType >= ETOUI(MATERIAL_TYPE::END))
		return;

	m_Table.variants[iMatIdx][iType] = iSlot;
}

_bool CRenderProfile::Has_Conflict(_uint iMatIdx, const CRenderRule::RESOLVE_RESULT& Result) const
{
	if (iMatIdx >= m_Table.passes.size() || iMatIdx >= m_Table.variants.size())
		return true;

	if (m_Table.passes[iMatIdx] != Result.iPass)
		return true;

	for (const auto& Override : Result.Variants)
	{
		const _uint iType = ETOUI(Override.eType);
		if (iType >= ETOUI(MATERIAL_TYPE::END))
			return true;

		if (m_Table.variants[iMatIdx][iType] != Override.iSlot)
			return true;
	}

	if (m_Table.useLayerColors[iMatIdx] != Result.bHasLayerColors)
		return true;

	if (true == Result.bHasLayerColors)
	{
		for (_uint i = 0; i < 4; ++i)
		{
			const _float4& A = m_Table.layerColors[iMatIdx][i];
			const _float4& B = Result.LayerColors[i];

			if (A.x != B.x || A.y != B.y || A.z != B.z || A.w != B.w)
				return true;
		}
	}

	return false;
}

void CRenderProfile::Apply_Result(_uint iMatIdx, const CRenderRule::RESOLVE_RESULT& Result)
{
	if (iMatIdx >= m_Table.passes.size() || iMatIdx >= m_Table.variants.size())
		return;

	m_Table.passes[iMatIdx] = Result.iPass;

	for (const auto& Override : Result.Variants)
	{
		const _uint iType = ETOUI(Override.eType);
		if (iType >= ETOUI(MATERIAL_TYPE::END))
			continue;

		m_Table.variants[iMatIdx][iType] = Override.iSlot;
	}

	if (true == Result.bHasLayerColors)
	{
		m_Table.useLayerColors[iMatIdx] = true;
		m_Table.layerColors[iMatIdx] = Result.LayerColors;
	}
}