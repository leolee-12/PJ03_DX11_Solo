#include "EffectEditorSession.h"
#include <filesystem>
#include <fstream>

#include "Effect.h"
#include "Effect_Manager.h"

#include "GameInstance.h"

CEffectEditorSession::CEffectEditorSession()
{
}

HRESULT CEffectEditorSession::Initialize()
{
	New_Doc();
	return S_OK;
}

void CEffectEditorSession::Update(_float fTimeDelta)
{
	(void)fTimeDelta;

	if (nullptr != m_pPreviewEffect && m_pPreviewEffect->Is_Dead())
		m_pPreviewEffect = nullptr;

	if (m_bPreviewRefreshPending)
	{
		m_bPreviewRefreshPending = false;

		const _string strReason = m_strStatus;
		if (SUCCEEDED(Spawn_Preview()))
		{
			m_strStatus = strReason.empty()
				? "Preview refreshed"
				: "Preview refreshed: " + strReason;
		}
	}
}

void CEffectEditorSession::Set_DocID(const _string& strID)
{
	if (m_Doc.strID == strID)
		return;

	m_Doc.strID = strID;
	Mark_Dirty("Effect id changed");
}

HRESULT CEffectEditorSession::Save(const _string& strPath)
{
	if (strPath.empty())
	{
		m_strStatus = "Save failed: empty path";
		return E_FAIL;
	}

	std::error_code ec;
	const std::filesystem::path path(strPath);

	if (path.has_parent_path())
		std::filesystem::create_directories(path.parent_path(), ec);

	if (std::filesystem::exists(path, ec))
	{
		const _string strBackup = strPath + ".bak";
		std::filesystem::copy_file(path, strBackup,
			std::filesystem::copy_options::overwrite_existing, ec);
	}

	std::ofstream ofs(strPath);
	if (!ofs.is_open())
	{
		m_strStatus = "Save failed: " + strPath;
		return E_FAIL;
	}

	ofs << Effect_SerializeDefinitionJson(m_Doc).dump(4);

	m_strDocPath = strPath;
	m_strStatus = "Saved: " + strPath;
	Clear_Dirty();
	return S_OK;
}

HRESULT CEffectEditorSession::Load(const _string& strPath)
{
	if (strPath.empty())
	{
		m_strStatus = "Load failed: empty path";
		return E_FAIL;
	}

	std::ifstream ifs(strPath);
	if (!ifs.is_open())
	{
		m_strStatus = "Load failed: " + strPath;
		return E_FAIL;
	}

	json root = json::parse(ifs, nullptr, false);
	if (root.is_discarded())
	{
		m_strStatus = "Load failed: invalid json";
		return E_FAIL;
	}

	EFFECT_DEFINITION loaded{};
	if (FAILED(Effect_ParseDefinitionJson(root, loaded)))
	{
		m_strStatus = "Load failed: invalid effect definition";
		return E_FAIL;
	}

	const _bool bPreviewWasAlive = Is_PreviewAlive();
	Destroy_Preview();

	m_Doc = std::move(loaded);
	m_strDocPath = strPath;
	m_iSelectedEmitter = -1;
	m_iSelectedMesh = -1;
	Normalize_Selection();
	Clear_Dirty();

	m_strStatus = "Loaded: " + strPath;

	if (bPreviewWasAlive && (!m_Doc.Emitters.empty() || !m_Doc.Meshes.empty() || !m_Doc.Trails.empty()))
		m_bPreviewRefreshPending = true;

	return S_OK;
}

void CEffectEditorSession::Clear_Dirty()
{
	m_bDirty = false;
}

void CEffectEditorSession::Set_DocPath(const _string& strPath)
{
	if (m_strDocPath == strPath)
		return;

	m_strDocPath = strPath;
	m_strStatus = "Path changed";
}

void CEffectEditorSession::Set_SelectedEmitter(_int iIndex)
{
	m_iSelectedEmitter = iIndex;
	Normalize_Selection();
}

void CEffectEditorSession::Set_SelectedMesh(_int iIndex)
{
	m_iSelectedMesh = iIndex;
	Normalize_Selection();
}

void CEffectEditorSession::Set_SelectedTrail(_int iIndex)
{
	m_iSelectedTrail = iIndex;
	Normalize_Selection();
}

void CEffectEditorSession::New_Doc()
{
	Destroy_Preview();

	m_Doc = {};
	m_Doc.strID = "NEW_EFFECT";
	m_iSelectedEmitter = -1;
	m_iSelectedMesh = -1;
	m_bDirty = false;
	m_strStatus = "New effect document";
}

void CEffectEditorSession::Add_Emitter()
{
	EMITTER_DEFINITION emitter{};
	emitter.strName = "emitter";
	m_Doc.Emitters.push_back(emitter);
	m_iSelectedEmitter = static_cast<_int>(m_Doc.Emitters.size()) - 1;
	Mark_Dirty("Emitter added");
}

void CEffectEditorSession::Erase_SelectedEmitter()
{
	if (m_iSelectedEmitter < 0 ||
		m_iSelectedEmitter >= static_cast<_int>(m_Doc.Emitters.size()))
		return;

	m_Doc.Emitters.erase(m_Doc.Emitters.begin() + m_iSelectedEmitter);
	Normalize_Selection();
	Mark_Dirty("Emitter removed");
}

EMITTER_DEFINITION* CEffectEditorSession::Get_SelectedEmitterMutable()
{
	if (m_iSelectedEmitter < 0 ||
		m_iSelectedEmitter >= static_cast<_int>(m_Doc.Emitters.size()))
		return nullptr;

	return &m_Doc.Emitters[m_iSelectedEmitter];
}

void CEffectEditorSession::Add_Mesh()
{
	MESH_EFFECT_DEFINITION mesh{};
	mesh.strName = "mesh";
	m_Doc.Meshes.push_back(mesh);
	m_iSelectedMesh = static_cast<_int>(m_Doc.Meshes.size()) - 1;
	Mark_Dirty("Mesh added");
}

void CEffectEditorSession::Erase_SelectedMesh()
{
	if (m_iSelectedMesh < 0 ||
		m_iSelectedMesh >= static_cast<_int>(m_Doc.Meshes.size()))
		return;

	m_Doc.Meshes.erase(m_Doc.Meshes.begin() + m_iSelectedMesh);
	Normalize_Selection();
	Mark_Dirty("Mesh removed");
}

MESH_EFFECT_DEFINITION* CEffectEditorSession::Get_SelectedMeshMutable()
{
	if (m_iSelectedMesh < 0 ||
		m_iSelectedMesh >= static_cast<_int>(m_Doc.Meshes.size()))
		return nullptr;

	return &m_Doc.Meshes[m_iSelectedMesh];
}

void CEffectEditorSession::Add_Trail()
{
	TRAIL_DEFINITION trail{};
	trail.strName = "trail";
	m_Doc.Trails.push_back(trail);
	m_iSelectedTrail = static_cast<_int>(m_Doc.Trails.size()) - 1;
	Mark_Dirty("Trail added");
}

void CEffectEditorSession::Erase_SelectedTrail()
{
	if (m_iSelectedTrail < 0 ||
		m_iSelectedTrail >= static_cast<_int>(m_Doc.Trails.size()))
		return;

	m_Doc.Trails.erase(m_Doc.Trails.begin() + m_iSelectedTrail);
	Normalize_Selection();
	Mark_Dirty("Trail removed");
}

TRAIL_DEFINITION* CEffectEditorSession::Get_SelectedTrailMutable()
{
	if (m_iSelectedTrail < 0 ||
		m_iSelectedTrail >= static_cast<_int>(m_Doc.Trails.size()))
		return nullptr;

	return &m_Doc.Trails[m_iSelectedTrail];
}

void CEffectEditorSession::Mark_Dirty(const char* pReason)
{
	const _bool bRefreshPreview = Is_PreviewAlive();

	m_bDirty = true;
	m_strStatus = (nullptr != pReason) ? pReason : "Modified";

	if (bRefreshPreview)
	{
		m_bPreviewRefreshPending = true;
	}
	else
	{
		/* Preview refresh is only automatic while a preview instance is alive. */
		m_strStatus += " (press Spawn Preview to apply)";
	}
}

HRESULT CEffectEditorSession::Spawn_Preview()
{
	m_bPreviewRefreshPending = false;
	Destroy_Preview();

	if (m_Doc.strID.empty())
	{
		m_strStatus = "Preview failed: empty effect id";
		return E_FAIL;
	}

	if (m_Doc.Emitters.empty() && m_Doc.Meshes.empty())
	{
		m_strStatus = "Preview failed: no emitter or mesh";
		return E_FAIL;
	}

	if (m_Doc.Emitters.empty() && m_Doc.Meshes.empty() && m_Doc.Trails.empty())
	{
		m_strStatus = "Preview failed: no emitter, mesh or trail";
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	const _uint iLevel = static_cast<_uint>(pGameInstance->Get_CurrentLevel());

	if (iLevel != ETOUI(LEVEL::GAMEPLAY))
	{
		m_strStatus = "Preview requires GAMEPLAY level";
		return E_FAIL;
	}

	const _uint iProtoLevel = ETOUI(LEVEL::STATIC);
	if (!pGameInstance->Has_Prototype(iProtoLevel, PROTO_OBJ_EFFECT))
	{
		m_strStatus = "Preview failed: effect prototype is not loaded";
		return E_FAIL;
	}

	if (!m_Doc.Emitters.empty() &&
		!pGameInstance->Has_Prototype(iProtoLevel, PROTO_OBJ_PARTICLE_EMITTER))
	{
		m_strStatus = "Preview failed: particle emitter prototype is not loaded";
		return E_FAIL;
	}

	if (!m_Doc.Meshes.empty() &&
		!pGameInstance->Has_Prototype(iProtoLevel, PROTO_OBJ_EFFECT_MESH))
	{
		m_strStatus = "Preview failed: effect mesh prototype is not loaded";
		return E_FAIL;
	}

	if (!m_Doc.Trails.empty() &&
		!pGameInstance->Has_Prototype(iProtoLevel, PROTO_OBJ_TRAIL))
	{
		m_strStatus = "Preview failed: trail prototype is not loaded";
		return E_FAIL;
	}

	Game_PKM::CEffect_Manager::GetInstance()->Register_Definition(m_Doc);

	m_pPreviewEffect = Game_PKM::CEffect_Manager::GetInstance()->PlayAt(
		m_Doc.strID,
		m_vPreviewPosition,
		iLevel,
		LAYER_EFFECT);

	if (nullptr == m_pPreviewEffect)
	{
		m_strStatus = "Preview spawn failed";
		return E_FAIL;
	}

	m_strStatus = "Preview spawned";
	return S_OK;
}

void CEffectEditorSession::Stop_Preview()
{
	m_bPreviewRefreshPending = false;

	if (nullptr == m_pPreviewEffect || m_pPreviewEffect->Is_Dead())
	{
		m_pPreviewEffect = nullptr;
		m_strStatus = "Preview is not active";
		return;
	}

	m_pPreviewEffect->Stop();
	m_pPreviewEffect = nullptr;
	m_strStatus = "Preview stopped";
}

void CEffectEditorSession::Destroy_Preview()
{
	m_bPreviewRefreshPending = false;

	if (nullptr != m_pPreviewEffect && !m_pPreviewEffect->Is_Dead())
		m_pPreviewEffect->Destroy();

	m_pPreviewEffect = nullptr;
}

_bool CEffectEditorSession::Is_PreviewAlive() const
{
	return nullptr != m_pPreviewEffect && !m_pPreviewEffect->Is_Dead();
}

void CEffectEditorSession::Set_PreviewPosition(const _float3& vPosition)
{
	m_vPreviewPosition = vPosition;
	m_strStatus = "Preview position changed";

	if (Is_PreviewAlive())
		m_bPreviewRefreshPending = true;
}

void CEffectEditorSession::Reset_PreviewPosition()
{
	Set_PreviewPosition(_float3(0.f, 1.f, 0.f));
}

void CEffectEditorSession::Normalize_Selection()
{
	if (m_Doc.Emitters.empty())
	{
		m_iSelectedEmitter = -1;
	}
	else
	{
		if (m_iSelectedEmitter < 0)
			m_iSelectedEmitter = 0;

		const _int iLastEmitter = static_cast<_int>(m_Doc.Emitters.size()) - 1;
		if (m_iSelectedEmitter > iLastEmitter)
			m_iSelectedEmitter = iLastEmitter;
	}

	if (m_Doc.Meshes.empty())
	{
		m_iSelectedMesh = -1;
	}
	else
	{
		if (m_iSelectedMesh < 0)
			m_iSelectedMesh = 0;

		const _int iLastMesh = static_cast<_int>(m_Doc.Meshes.size()) - 1;
		if (m_iSelectedMesh > iLastMesh)
			m_iSelectedMesh = iLastMesh;
	}

	if (m_Doc.Trails.empty())
	{
		m_iSelectedTrail = -1;
	}
	else
	{
		if (m_iSelectedTrail < 0)
			m_iSelectedTrail = 0;

		const _int iLastTrail = static_cast<_int>(m_Doc.Trails.size()) - 1;
		if (m_iSelectedTrail > iLastTrail)
			m_iSelectedTrail = iLastTrail;
	}
}

CEffectEditorSession* CEffectEditorSession::Create()
{
	CEffectEditorSession* pInstance = new CEffectEditorSession();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CEffectEditorSession");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectEditorSession::Free()
{
	Destroy_Preview();

	__super::Free();
}