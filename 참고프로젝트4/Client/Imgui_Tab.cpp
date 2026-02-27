#include "stdafx.h"
#ifdef _DEBUG
#include "Imgui_Tab.h"

#include "GameInstance.h"
#include "Imgui_Manager.h"
#include "ImGuiFileDialog.h"

#include "Imgui_MapToolBase.h"




CImgui_Tab::CImgui_Tab(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_pContext(pContext), m_pDevice(pDevice)
	, m_pImgui_Manger(CImgui_Manager::GetInstance())
{
	for (_uint i = 0; i < (_uint)LAYERTYPE_END; i++)
		m_pGameObjectList[i] = &pGameObjectList[i];
}

//게임오브젝트가 없는 툴의 경우
CImgui_Tab::CImgui_Tab(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_pContext(pContext), m_pDevice(pDevice)
	, m_pImgui_Manger(CImgui_Manager::GetInstance())
{
}

HRESULT CImgui_Tab::Initialize()
{
	return S_OK;
}

void CImgui_Tab::Tick(_cref_time fTimeDelta)
{
}

HRESULT CImgui_Tab::Render()
{
	return S_OK;
}

void CImgui_Tab::Use_ImGuizmo(shared_ptr<CGameObject> pGameObject, _bool bModel)
{
	/*==== GridMode ====*/
	ImGui::Checkbox("Render_Grid_Mode", &m_bGridMode);

	/*==== Set ImGuizmo ====*/
	ImGuizmo::SetOrthographic(false);
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

	if (ImGui::IsKeyPressed(ImGuiKey_Z))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(ImGuiKey_X))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(ImGuiKey_C))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	_float* arrView = CImgui_Manager::GetInstance()->m_arrView;
	_float* arrProj = CImgui_Manager::GetInstance()->m_arrProj;

	_float4x4 matWorld;

	matWorld = pGameObject->Get_TransformCom().lock()->Get_WorldFloat4x4();

	_float arrWorld[] = { matWorld._11,matWorld._12,matWorld._13,matWorld._14,
						  matWorld._21,matWorld._22,matWorld._23,matWorld._24,
						  matWorld._31,matWorld._32,matWorld._33,matWorld._34,
						  matWorld._41,matWorld._42,matWorld._43,matWorld._44 };

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(arrWorld, matrixTranslation, matrixRotation, matrixScale);
	ImGui::PushItemWidth(200);

	if (matrixRotation[0] == NAN)
		matrixRotation[0] = 0.0f;
	if (matrixRotation[1] == NAN)
		matrixRotation[1] = 0.0f;
	if (matrixRotation[2] == NAN)
		matrixRotation[2] = 0.0f;

	if (matrixScale[0] == NAN)
		matrixScale[0] = 0.001f;
	if (matrixScale[1] == NAN)
		matrixScale[1] = 0.001f;
	if (matrixScale[2] == NAN)
		matrixScale[2] = 0.001f;

	if (bModel)
	{
		if (mCurrentGizmoOperation == ImGuizmo::TRANSLATE)
		{
			ImGui::DragFloat3("Tr ", matrixTranslation);
			ImGui::SeparatorText("Arrow ");
			ImGui::SetNextItemWidth(100.0f);
			ImGui::DragFloat("Arrow_Value ", &m_fAddArrow);
			m_pImgui_Manger->Arrow_Button("TrX ", m_fAddArrow, &matrixTranslation[0]);
			Change_Value("TrX", m_fAddArrow, &matrixTranslation[0]);
			m_pImgui_Manger->Arrow_Button("TrY ", m_fAddArrow, &matrixTranslation[1]);
			Change_Value("TrY", m_fAddArrow, &matrixTranslation[1]);
			m_pImgui_Manger->Arrow_Button("TrZ ", m_fAddArrow, &matrixTranslation[2]);
			Change_Value("TrZ", m_fAddArrow, &matrixTranslation[2]);

		}
	}
	else
	{
		if (mCurrentGizmoOperation == ImGuizmo::TRANSLATE)
		{
			ImGui::DragFloat3("Offset", &m_vTesOffset.x, 0.5f);
			ImGui::SeparatorText("Arrow");
			ImGui::SetNextItemWidth(100.0f);
			ImGui::DragFloat("Arrow_Value", &m_fAddArrow);
			m_pImgui_Manger->Arrow_Button("OffSetX ", m_fAddArrow, &m_vTesOffset.x);
			Change_Value("OffSetX", m_fAddArrow, &m_vTesOffset.x);
			m_pImgui_Manger->Arrow_Button("OffSetY ", m_fAddArrow, &m_vTesOffset.y);
			Change_Value("OffSetY", m_fAddArrow, &m_vTesOffset.y);
			m_pImgui_Manger->Arrow_Button("OffSetZ ", m_fAddArrow, &m_vTesOffset.z);
			Change_Value("OffSetZ", m_fAddArrow, &m_vTesOffset.z);
		}
	}

	if (mCurrentGizmoOperation == ImGuizmo::ROTATE)
	{
		ImGui::DragFloat3("Rt", matrixRotation);
		ImGui::SeparatorText("Arrow");
		ImGui::SetNextItemWidth(100.0f);
		ImGui::DragFloat("Arrow_Value", &m_fAddArrow);
		m_pImgui_Manger->Arrow_Button("RotX ", m_fAddArrow, &matrixRotation[0]);
		Change_Value("RotX", m_fAddArrow, &matrixRotation[0]);
		m_pImgui_Manger->Arrow_Button("RotY ", m_fAddArrow, &matrixRotation[1]);
		Change_Value("RotY", m_fAddArrow, &matrixRotation[1]);
		m_pImgui_Manger->Arrow_Button("RotZ ", m_fAddArrow, &matrixRotation[2]);
		Change_Value("RotZ", m_fAddArrow, &matrixRotation[2]);
	}
	if (mCurrentGizmoOperation == ImGuizmo::SCALE)
	{
		ImGui::DragFloat3("Sc", matrixScale, 0.001f);
		ImGui::SeparatorText("Arrow");
		ImGui::SetNextItemWidth(100.0f);
		ImGui::DragFloat("Arrow_Value", &m_fAddArrow);
		m_pImgui_Manger->Arrow_Button("ScX ", m_fAddArrow, &matrixScale[0]);
		Change_Value("ScX", m_fAddArrow, &matrixScale[0]);
		m_pImgui_Manger->Arrow_Button("ScY ", m_fAddArrow, &matrixScale[1]);
		Change_Value("ScY", m_fAddArrow, &matrixScale[1]);
		m_pImgui_Manger->Arrow_Button("ScZ ", m_fAddArrow, &matrixScale[2]);
		Change_Value("ScZ", m_fAddArrow, &matrixScale[2]);
	}

	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, arrWorld);
	ImGui::PopItemWidth();


	ImGui::Checkbox("UseSnap", &useSnap);

	if (useSnap)
	{
		ImGui::SameLine();
		switch (mCurrentGizmoOperation)
		{
		case ImGuizmo::TRANSLATE:
			ImGui::DragFloat3("Snap", &snap[0]);
			break;
		case ImGuizmo::ROTATE:
			ImGui::DragFloat3("Angle Snap", &snap[0]);
			break;
		case ImGuizmo::SCALE:
			ImGui::DragFloat3("Scale Snap", &snap[0]);
			break;
		}
	}

	if (m_bGridMode)
		CImgui_Manager::GetInstance()->DrawGrid();
	ImGuizmo::Manipulate(arrView, arrProj, mCurrentGizmoOperation, mCurrentGizmoMode, arrWorld, NULL, useSnap ? &snap[0] : NULL);

	_float4x4 matW = { float4(arrWorld[0],arrWorld[1],arrWorld[2],arrWorld[3]),
					float4(arrWorld[4],arrWorld[5],arrWorld[6],arrWorld[7]),
					float4(arrWorld[8],arrWorld[9],arrWorld[10],arrWorld[11]),
					float4(arrWorld[12],arrWorld[13],arrWorld[14],arrWorld[15]) };

	pGameObject->Get_TransformCom().lock()->Set_WorldFloat4x4(matW);

	if (ImGuizmo::IsOver())
	{
		int a = 0;
	}
}

void CImgui_Tab::Use_ImGuizmo(_float4x4& Transform, _bool bModel)
{
	/*==== GridMode ====*/
	ImGui::Checkbox("Render_Grid_Mode", &m_bGridMode);

	/*==== Set ImGuizmo ====*/
	ImGuizmo::SetOrthographic(false);
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

	if (ImGui::IsKeyPressed(ImGuiKey_Z))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(ImGuiKey_X))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(ImGuiKey_C))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	_float* arrView = CImgui_Manager::GetInstance()->m_arrView;
	_float* arrProj = CImgui_Manager::GetInstance()->m_arrProj;

	_float4x4 matWorld = Transform;

	_float arrWorld[] = { matWorld._11,matWorld._12,matWorld._13,matWorld._14,
						  matWorld._21,matWorld._22,matWorld._23,matWorld._24,
						  matWorld._31,matWorld._32,matWorld._33,matWorld._34,
						  matWorld._41,matWorld._42,matWorld._43,matWorld._44 };

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(arrWorld, matrixTranslation, matrixRotation, matrixScale);
	ImGui::PushItemWidth(200);

	if (matrixRotation[0] == NAN)
		matrixRotation[0] = 0.0f;
	if (matrixRotation[1] == NAN)
		matrixRotation[1] = 0.0f;
	if (matrixRotation[2] == NAN)
		matrixRotation[2] = 0.0f;

	if (matrixScale[0] == NAN)
		matrixScale[0] = 0.001f;
	if (matrixScale[1] == NAN)
		matrixScale[1] = 0.001f;
	if (matrixScale[2] == NAN)
		matrixScale[2] = 0.001f;

	if (bModel)
	{
		if (mCurrentGizmoOperation == ImGuizmo::TRANSLATE)
		{
			ImGui::DragFloat3("Tr ", matrixTranslation);
			ImGui::SeparatorText("Arrow ");
			ImGui::SetNextItemWidth(100.0f);
			ImGui::DragFloat("Arrow_Value ", &m_fAddArrow);
			m_pImgui_Manger->Arrow_Button("TrX ", m_fAddArrow, &matrixTranslation[0]);
			Change_Value("TrX", m_fAddArrow, &matrixTranslation[0]);
			m_pImgui_Manger->Arrow_Button("TrY ", m_fAddArrow, &matrixTranslation[1]);
			Change_Value("TrY", m_fAddArrow, &matrixTranslation[1]);
			m_pImgui_Manger->Arrow_Button("TrZ ", m_fAddArrow, &matrixTranslation[2]);
			Change_Value("TrZ", m_fAddArrow, &matrixTranslation[2]);

		}
	}
	else
	{
		if (mCurrentGizmoOperation == ImGuizmo::TRANSLATE)
		{
			ImGui::DragFloat3("Offset", &m_vTesOffset.x, 0.5f);
			ImGui::SeparatorText("Arrow");
			ImGui::SetNextItemWidth(100.0f);
			ImGui::DragFloat("Arrow_Value", &m_fAddArrow);
			m_pImgui_Manger->Arrow_Button("OffSetX ", m_fAddArrow, &m_vTesOffset.x);
			Change_Value("OffSetX", m_fAddArrow, &m_vTesOffset.x);
			m_pImgui_Manger->Arrow_Button("OffSetY ", m_fAddArrow, &m_vTesOffset.y);
			Change_Value("OffSetY", m_fAddArrow, &m_vTesOffset.y);
			m_pImgui_Manger->Arrow_Button("OffSetZ ", m_fAddArrow, &m_vTesOffset.z);
			Change_Value("OffSetZ", m_fAddArrow, &m_vTesOffset.z);
		}
	}

	if (mCurrentGizmoOperation == ImGuizmo::ROTATE)
	{
		ImGui::DragFloat3("Rt", matrixRotation);
		ImGui::SeparatorText("Arrow");
		ImGui::SetNextItemWidth(100.0f);
		ImGui::DragFloat("Arrow_Value", &m_fAddArrow);
		m_pImgui_Manger->Arrow_Button("RotX ", m_fAddArrow, &matrixRotation[0]);
		Change_Value("RotX", m_fAddArrow, &matrixRotation[0]);
		m_pImgui_Manger->Arrow_Button("RotY ", m_fAddArrow, &matrixRotation[1]);
		Change_Value("RotY", m_fAddArrow, &matrixRotation[1]);
		m_pImgui_Manger->Arrow_Button("RotZ ", m_fAddArrow, &matrixRotation[2]);
		Change_Value("RotZ", m_fAddArrow, &matrixRotation[2]);
	}
	if (mCurrentGizmoOperation == ImGuizmo::SCALE)
	{
		ImGui::DragFloat3("Sc", matrixScale, 0.001f);
		ImGui::SeparatorText("Arrow");
		ImGui::SetNextItemWidth(100.0f);
		ImGui::DragFloat("Arrow_Value", &m_fAddArrow);
		m_pImgui_Manger->Arrow_Button("ScX ", m_fAddArrow, &matrixScale[0]);
		Change_Value("ScX", m_fAddArrow, &matrixScale[0]);
		m_pImgui_Manger->Arrow_Button("ScY ", m_fAddArrow, &matrixScale[1]);
		Change_Value("ScY", m_fAddArrow, &matrixScale[1]);
		m_pImgui_Manger->Arrow_Button("ScZ ", m_fAddArrow, &matrixScale[2]);
		Change_Value("ScZ", m_fAddArrow, &matrixScale[2]);
	}

	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, arrWorld);
	ImGui::PopItemWidth();


	ImGui::Checkbox("UseSnap", &useSnap);

	if (useSnap)
	{
		ImGui::SameLine();
		switch (mCurrentGizmoOperation)
		{
		case ImGuizmo::TRANSLATE:
			ImGui::DragFloat3("Snap", &snap[0]);
			break;
		case ImGuizmo::ROTATE:
			ImGui::DragFloat3("Angle Snap", &snap[0]);
			break;
		case ImGuizmo::SCALE:
			ImGui::DragFloat3("Scale Snap", &snap[0]);
			break;
		}
	}

	if (m_bGridMode)
		CImgui_Manager::GetInstance()->DrawGrid();
	ImGuizmo::Manipulate(arrView, arrProj, mCurrentGizmoOperation, mCurrentGizmoMode, arrWorld, NULL, useSnap ? &snap[0] : NULL);

	_float4x4 matW = { float4(arrWorld[0],arrWorld[1],arrWorld[2],arrWorld[3]),
					float4(arrWorld[4],arrWorld[5],arrWorld[6],arrWorld[7]),
					float4(arrWorld[8],arrWorld[9],arrWorld[10],arrWorld[11]),
					float4(arrWorld[12],arrWorld[13],arrWorld[14],arrWorld[15]) };

	Transform = matW;

	if (ImGuizmo::IsOver())
	{
		int a = 0;
	}
}

void CImgui_Tab::Change_Value(string szName, _float fValue, _float* fOrigin)
{
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.f);
	if (ImGui::Button(szName.c_str()))
		*fOrigin = fValue;
}

void CImgui_Tab::HelpMarker(const char* desc)
{
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(desc);
		ImGui::EndTooltip();
	}
}

void CImgui_Tab::Free()
{
	Safe_Delete(m_pDesc);
}

#endif // DEBUG
