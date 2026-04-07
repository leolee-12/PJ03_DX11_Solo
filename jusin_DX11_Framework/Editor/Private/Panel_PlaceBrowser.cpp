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

	Register_Items();
	return S_OK;
}

void CPanel_PlaceBrowser::Update(_float fTimeDelta)
{
}

HRESULT CPanel_PlaceBrowser::Render()
{
	if (!Begin_Panel()) { End_Panel(); return S_OK; }

	ImGui::InputText("##filter", m_szFilter, 128);
	ImGui::SameLine();
	if (ImGui::Button(KOR("초기화"))) m_szFilter[0] = '\0';
	ImGui::Separator();

	static const _char* ppCategories[] = { KOR("UI"), KOR("맵"),KOR("맵OBJ"), KOR("NPC"), KOR("몬스터"), KOR("아이템")};

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
	{ ETOUI(LEVEL::LOGO),		PROTO_OBJ_BACKGROUND,	ETOUI(LEVEL::LOGO),		LAYER_BACKGROUND,	KOR("UI_Default"),		KOR("UI")},
	{ ETOUI(LEVEL::GAMEPLAY),	PROTO_OBJ_TERRAIN,		ETOUI(LEVEL::GAMEPLAY),	LAYER_BACKGROUND,	KOR("맵_Default"),		KOR("맵") },
	{ ETOUI(LEVEL::GAMEPLAY),	PROTO_OBJ_FORKLIFT,		ETOUI(LEVEL::GAMEPLAY),	LAYER_BACKGROUND,	KOR("포크리프트"),		KOR("몬스터") },
	{ ETOUI(LEVEL::GAMEPLAY),	PROTO_OBJ_MONSTER,		ETOUI(LEVEL::GAMEPLAY),	LAYER_MONSTER,		KOR("피오나"),			KOR("몬스터") },
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

		const _bool bSelectedPlaceMode =
			m_pEditInstance->Is_PlaceMode() &&
			m_pEditInstance->Get_PlaceItem().strProtoTag == pItem->strProtoTag &&
			m_pEditInstance->Get_PlaceItem().iProtoLevel == pItem->iProtoLevel;

		bool bClicked = ImGui::Selectable(
			pItem->strDisplayName.c_str(),
			bSelectedPlaceMode,
			ImGuiSelectableFlags_AllowDoubleClick);

		if (bClicked)
		{
			if (bSelectedPlaceMode)
				m_pEditInstance->End_PlaceMode();
			else
				m_pEditInstance->Begin_PlaceMode(*pItem);
		}

		if (bClicked && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			Place_Object(*pItem);

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("PLACE_ITEM", &pItem, sizeof(CATALOG_ITEM*));
			ImGui::Text(KOR("배치: %s"), pItem->strDisplayName.c_str());
			ImGui::EndDragDropSource();
		}
	}
}

void CPanel_PlaceBrowser::Place_Object(const CATALOG_ITEM tItem)
{
	// EditInstance → Object_Registry -> Clone + Add_Obj_Ex + 추적
	m_pEditInstance->Register_Object(
		tItem.iProtoLevel, tItem.strProtoTag,
		tItem.iLayerLevel, tItem.strLayerTag, nullptr);

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
