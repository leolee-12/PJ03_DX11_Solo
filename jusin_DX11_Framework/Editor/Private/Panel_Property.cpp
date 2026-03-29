#include "Panel_Property.h"
#include "EditInstance.h"

CPanel_Property::CPanel_Property()
	: CPanel_Base()
{
}

HRESULT CPanel_Property::Initialize()
{
	m_strTitle = "Property";

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	Register_Drawers();

	return S_OK;
}

void CPanel_Property::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

HRESULT CPanel_Property::Render()
{
	if (!Begin_Panel()) { End_Panel(); return S_OK; }

	if (!m_pSelected)
	{
		ImGui::TextDisabled("Not Selected");
		End_Panel();
		return S_OK;
	}

	// 선택 객체가 바뀔 때만 버퍼 동기화
	if (m_pSelected != m_pLastSelected)
	{
		strncpy_s(m_szNameBuffer, WtoS(m_pSelected->Get_Name()).c_str(), 255);
		m_pLastSelected = m_pSelected;
	}

	if (ImGui::InputText("##objname", m_szNameBuffer, 256, ImGuiInputTextFlags_EnterReturnsTrue))
		m_pSelected->Set_Name(StoW(m_szNameBuffer));

	ImGui::Separator();

	if (!m_pSelected->Is_UI())
	{
		Draw_Transform(m_pSelected->Get_Transform());
		ImGui::Separator();
	}

	Draw_TypeProps(m_pSelected);

	End_Panel();
	return S_OK;
}

void CPanel_Property::Draw_Transform(CTransform* pTransformCom)
{
	if (!pTransformCom) return;

	if (!ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		return;

	_float fPos[3], fRot[3], fScale[3];
	ImGuizmo::DecomposeMatrixToComponents(
		reinterpret_cast<const _float*>(pTransformCom->Get_WorldMatrixPtr()), fPos, fRot, fScale);

	for (int i = 0; i < 3; ++i)
	{
		if (!_finite(fScale[i]) || fScale[i] < 1e-6f) fScale[i] = 1.f;
		if (!_finite(fRot[i]))  fRot[i] = 0.f;
		if (!_finite(fPos[i]))  fPos[i] = 0.f;
	}

	bool bChanged = false;
	bChanged |= ImGui::DragFloat3(KOR("위치"), fPos, 0.05f);
	bChanged |= ImGui::DragFloat3(KOR("회전"), fRot, 0.5f, -360.f, 360.f, "%.1f°");
	bChanged |= ImGui::DragFloat3(KOR("스케일"), fScale, 0.01f, 0.001f, 100.f);

	if (bChanged)
	{
		_float newMatrix[16];
		ImGuizmo::RecomposeMatrixFromComponents(fPos, fRot, fScale, newMatrix);

		_float4x4 worldMatrix;
		memcpy(&worldMatrix, newMatrix, sizeof(_float) * 16);
		pTransformCom->Set_State(STATE::RIGHT, XMLoadFloat4(reinterpret_cast<_float4*>(&worldMatrix._11)));
		pTransformCom->Set_State(STATE::UP, XMLoadFloat4(reinterpret_cast<_float4*>(&worldMatrix._21)));
		pTransformCom->Set_State(STATE::LOOK, XMLoadFloat4(reinterpret_cast<_float4*>(&worldMatrix._31)));
		pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(reinterpret_cast<_float4*>(&worldMatrix._41)));
	}
}

void CPanel_Property::Draw_TypeProps(CGameObject* pObj)
{
	_string typeName = pObj->Get_TypeName();
	auto iter = m_DrawerTable.find(typeName);

	if (iter != m_DrawerTable.end())
		iter->second(pObj);
}

void CPanel_Property::Register_Drawers()
{
}

CPanel_Property* CPanel_Property::Create()
{
	CPanel_Property* pInstance = new CPanel_Property();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CPanel_Property");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPanel_Property::Free()
{
	__super::Free();
}
