#include "Panel_Model.h"
#include "GameInstance.h"
#include "EditInstance.h"

CPanel_Model::CPanel_Model()
	: CPanel_Base()
{
}

HRESULT CPanel_Model::Initialize()
{
	m_strTitle = "Model";

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	return S_OK;
}

void CPanel_Model::Update(_float fTimeDelta)
{
}

HRESULT CPanel_Model::Render()
{
    if (!Begin_Panel()) { End_Panel(); return S_OK; }

	/* ── 1. FBX 파일 선택 ───────────────────────── */
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 36.f);
	ImGui::InputText("##fbx_path", m_szFbxPath, MAX_PATH, ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine(0.f, 4.f);
	if (ImGui::Button("...##fbx"))
	{
		Open_FileDialog(m_szFbxPath, MAX_PATH, "FBX Files\0*.fbx\0All Files\0*.*\0");
	}

	/* ── 2. 모델 타입 선택 ──────────────────────── */
	_int iType = ETOI(m_eType);
	ImGui::Text("Type :");
	ImGui::SameLine();
	ImGui::RadioButton("NONANIM", &iType, ETOI(MODEL::NONANIM));
	ImGui::SameLine();
	ImGui::RadioButton("ANIM", &iType, ETOI(MODEL::ANIM));
	m_eType = static_cast<MODEL>(iType);

	/* ── 3. PreTransform 파라미터 ───────────────── */
	ImGui::InputFloat("Scale", &m_fScale, 0.001f, 0.01f, "%.4f");
	ImGui::InputFloat("Rotation X", &m_fRotationX, 1.f, 10.f, "%.1f deg");
	ImGui::InputFloat("Rotation Y", &m_fRotationY, 1.f, 10.f, "%.1f deg");
	ImGui::InputFloat("Rotation Z", &m_fRotationZ, 1.f, 10.f, "%.1f deg");
	/* ── 4. Load 버튼 ───────────────────────────── */
	ImGui::BeginDisabled(m_szFbxPath[0] == '\0');
	if (ImGui::Button("[Load FBX]"))
	{
		_matrix PreTransform = XMMatrixScaling(m_fScale, m_fScale, m_fScale)
			* XMMatrixRotationX(XMConvertToRadians(m_fRotationX))
			* XMMatrixRotationY(XMConvertToRadians(m_fRotationY))
			* XMMatrixRotationZ(XMConvertToRadians(m_fRotationZ));
		
		m_pEditInstance->Load_FBX(m_szFbxPath, m_eType, PreTransform);
	}
	ImGui::EndDisabled();

	ImGui::Separator();

	/* ── 5. Loaded Info ─────────────────────────── */
	if (!m_pEditInstance->Is_ModelLoaded())
	{
		ImGui::TextDisabled("No model loaded.");
		End_Panel();
		return S_OK;
	}

	WMODEL_HEADER tHeader = m_pEditInstance->Get_ModelMetaData();

	ImGui::Text("Bones		: %zu", tHeader.iNumBones);
	ImGui::Text("Meshes		: %zu", tHeader.iNumMeshes);
	ImGui::Text("Materials	: %zu", tHeader.iNumMaterials);
	ImGui::Text("Animations : %zu", tHeader.iNumAnimations);

	ImGui::Spacing();

	/* ── 6. 본 목록 (스크롤) ────────────────────── */
	ImGui::Text("Bone List");
	if (ImGui::BeginChild("##Bone List", ImVec2(0, 160), ImGuiChildFlags_Borders))
	{
		const auto& Bones = m_pEditInstance->Get_ModelBones();

		for (size_t i = 0; i < Bones.size(); ++i)
		{
			ImGui::Text("[%3zu] %-30s (parent: %d)",
				i, Bones[i].szName, Bones[i].iParentIndex);
		}
	}

	ImGui::EndChild();

	ImGui::Separator();

	/* ── 7. Export ──────────────────────────────── */
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 36.f);
	ImGui::InputText("##out_dir", m_szOutputDir, MAX_PATH);
	ImGui::SameLine(0.f, 4.f);
	if (ImGui::Button("...##out"))
	{
		//Open_FileDialog(m_szOutputDir, MAX_PATH, "All Files\0*.*\0");
		Open_FolderDialog(m_szOutputDir, MAX_PATH);
	}

	ImGui::BeginDisabled(m_szOutputDir[0] == '\0');

	if (ImGui::Button("[Export Binary]", ImVec2(-1, 0)))
	{
		_matrix PreTransform = XMMatrixScaling(m_fScale, m_fScale, m_fScale)
			* XMMatrixRotationX(XMConvertToRadians(m_fRotationX))
			* XMMatrixRotationY(XMConvertToRadians(m_fRotationY))
			* XMMatrixRotationZ(XMConvertToRadians(m_fRotationZ));
		m_pEditInstance->Export_Binary(m_szFbxPath, m_szOutputDir, m_eType, PreTransform);
    }
    if (ImGui::Button("[Export All]", ImVec2(-1, 0)))   // Binary + Meta JSON
    {
		_matrix PreTransform = XMMatrixScaling(m_fScale, m_fScale, m_fScale)
			* XMMatrixRotationX(XMConvertToRadians(m_fRotationX))
			* XMMatrixRotationY(XMConvertToRadians(m_fRotationY))
			* XMMatrixRotationZ(XMConvertToRadians(m_fRotationZ));
		m_pEditInstance->Export_All(m_szFbxPath, m_szOutputDir, m_eType, PreTransform);
    }

	ImGui::EndDisabled();

    End_Panel();
    return S_OK;
}

CPanel_Model* CPanel_Model::Create()
{
	CPanel_Model* pInstance = new CPanel_Model();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CPanel_Model");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPanel_Model::Free()
{
	__super::Free();
}
