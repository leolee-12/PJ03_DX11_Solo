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

	_float pPos[3], pRot[3], pScale[3];
	ImGuizmo::DecomposeMatrixToComponents(
		reinterpret_cast<const _float*>(pTransformCom->Get_WorldMatrixPtr()), pPos, pRot, pScale);

	for (int i = 0; i < 3; ++i)
	{
		if (!_finite(pScale[i]) || pScale[i] < 1e-6f) pScale[i] = 1.f;
		if (!_finite(pRot[i]))  pRot[i] = 0.f;
		if (!_finite(pPos[i]))  pPos[i] = 0.f;
	}

	bool bChanged = false;
	bChanged |= ImGui::DragFloat3(KOR("위치"), pPos, 0.05f);
	bChanged |= ImGui::DragFloat3(KOR("회전"), pRot, 0.5f, -360.f, 360.f, "%.1f°");
	bChanged |= ImGui::DragFloat3(KOR("스케일"), pScale, 0.01f, 0.001f, 100.f);

	if (bChanged)
	{

		Set_WorldMatrix(pTransformCom, pPos, pRot, pScale);

		//_float newMatrix[16];
		//ImGuizmo::RecomposeMatrixFromComponents(pPos, pRot, pScale, newMatrix);

		//_float4x4 worldMatrix;
		//memcpy(&worldMatrix, newMatrix, sizeof(_float) * 16);
		//pTransformCom->Set_State(STATE::RIGHT, XMLoadFloat4(reinterpret_cast<_float4*>(&worldMatrix._11)));
		//pTransformCom->Set_State(STATE::UP, XMLoadFloat4(reinterpret_cast<_float4*>(&worldMatrix._21)));
		//pTransformCom->Set_State(STATE::LOOK, XMLoadFloat4(reinterpret_cast<_float4*>(&worldMatrix._31)));
		//pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(reinterpret_cast<_float4*>(&worldMatrix._41)));
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
