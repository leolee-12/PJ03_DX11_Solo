#include "Panel_PlaceBrowser.h"
#include "GameInstance.h"
#include "EditInstance.h"

CPanel_PlaceBrowser::CPanel_PlaceBrowser()
	: CPanel_Base()
{
}

HRESULT CPanel_PlaceBrowser::Initialize()
{
	m_strTitle = "PlaceBrowser";

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	m_iProtoLevel = ETOUI(LEVEL::STATIC);
	m_iLayerLevel = ETOUI(LEVEL::GAMEPLAY);
	m_LayerTag = WNAME(L"Layer_Object");

	Register_Items();
	return S_OK;
}

void CPanel_PlaceBrowser::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

HRESULT CPanel_PlaceBrowser::Render()
{
	if (!Begin_Panel()) { End_Panel(); return S_OK; }

	ImGui::InputText("##filter", m_szFilter, 128);
	ImGui::SameLine();
	if (ImGui::Button("초기화")) m_szFilter[0] = '\0';
	ImGui::Separator();

	static const _char* ppCategories[] = { "맵","NPC","포켓몬","아이템","몬스터"};

	if (ImGui::BeginTabBar("##cat"))
	{
		for (const _char* pCat : ppCategories)
		{
			if (ImGui::BeginTabItem(pCat))
			{
				Draw_Category(pCat);
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}

	End_Panel();

	return S_OK;
}

void CPanel_PlaceBrowser::Register_Items()
{
	m_AllItems =
	{
	{ WNAME(L"Prototype_Terrain"),		KOR("지형"),			KOR("맵") },
	{ WNAME(L"Prototype_Tree"),			KOR("나무"),			KOR("맵") },
	{ WNAME(L"Prototype_NPC_Shop"),		KOR("상점 NPC"),		KOR("NPC") },
	{ WNAME(L"Prototype_NPC_Guide"),	KOR("안내 NPC"),		KOR("NPC") },
	{ WNAME(L"Prototype_EncPoint"),		KOR("출현 포인트"),		KOR("포켓몬") },
	{ WNAME(L"Prototype_Item_Ball"),	KOR("포켓볼"),			KOR("아이템") },
	{ PROTO_OBJ_FORKLIFT,				KOR("포크리프트"),		KOR("몬스터") },
	{ PROTO_OBJ_MONSTER,				KOR("피오나"),			KOR("몬스터") },
	};

	for (auto& item : m_AllItems)
		m_ByCategory[item.strCategory].push_back(&item);
}

void CPanel_PlaceBrowser::Draw_Category(const _string& strCat)
{
	auto iter = m_ByCategory.find(strCat);
	if (iter == m_ByCategory.end())
		return;

	for (CATALOG_ITEM* pItem : iter->second)
	{
		if (m_szFilter[0] != '\0' &&
			pItem->strDisplayName.find(m_szFilter) == _string::npos)
			continue;

		bool bClicked = ImGui::Selectable(
			pItem->strDisplayName.c_str(), false,
			ImGuiSelectableFlags_AllowDoubleClick);

		if (bClicked && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			Place_Object(*pItem);

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("PLACE_ITEM", &pItem, sizeof(CATALOG_ITEM*));
			ImGui::Text("배치: %s", pItem->strDisplayName.c_str());
			ImGui::EndDragDropSource();
		}
	}
}

void CPanel_PlaceBrowser::Place_Object(const CATALOG_ITEM tItem)
{
	// EditInstance → Object_Registry -> Clone + Add_Obj_Ex + 추적
	m_pEditInstance->Register_Object(
		m_iProtoLevel, tItem.protoTag,
		m_iLayerLevel, m_LayerTag, nullptr);

	// 선택 상태로 전환 (프로퍼티 편집기 즉시 표시)
	auto& objs = m_pEditInstance->Get_EditorObjects();
	if (!objs.empty())
		m_pEditInstance->Select(objs.back(), false);
}

CPanel_PlaceBrowser* CPanel_PlaceBrowser::Create()
{
	CPanel_PlaceBrowser* pInstance = new CPanel_PlaceBrowser();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CPanel_PlaceBrowser");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPanel_PlaceBrowser::Free()
{
	__super::Free();
}
