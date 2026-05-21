#include "EventAction.h"
#include "Level_GamePlay.h"
#include "Camera_Free.h"
#include "Spawn_Manager.h"
#include "Actor_NPC.h"

#include "GameInstance.h"

#include <sstream>

namespace
{
	const _string* Find_Param_(const unordered_map<_string, _string>& Params, const _string& strKey)
	{
		auto iter = Params.find(strKey);
		if (iter == Params.end())
			return nullptr;

		return &iter->second;
	}

	_bool Parse_Float_(const unordered_map<_string, _string>& Params, const _string& strKey, _float& fOut)
	{
		const _string* pValue = Find_Param_(Params, strKey);
		if (nullptr == pValue)
			return false;

		try
		{
			fOut = std::stof(*pValue);
			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}

	_bool Parse_Float3_(const unordered_map<_string, _string>& Params, const _string& strKey, _float3& vOut)
	{
		const _string* pValue = Find_Param_(Params, strKey);
		if (nullptr == pValue)
			return false;

		stringstream ss(*pValue);
		string token;

		try
		{
			if (getline(ss, token, ','))
				vOut.x = std::stof(token);
			if (getline(ss, token, ','))
				vOut.y = std::stof(token);
			if (getline(ss, token, ','))
				vOut.z = std::stof(token);
		}
		catch (const std::exception&)
		{
			return false;
		}

		return true;
	}

	_string Read_String_(const unordered_map<_string, _string>& Params, const _string& strKey, const _string& strDefault = "")
	{
		const _string* pValue = Find_Param_(Params, strKey);
		if (nullptr == pValue)
			return strDefault;

		return *pValue;
	}

	_bool Read_Bool_(const unordered_map<_string, _string>& Params, const _string& strKey, _bool bDefault)
	{
		const _string* pValue = Find_Param_(Params, strKey);
		if (nullptr == pValue)
			return bDefault;

		if ("true" == *pValue || "True" == *pValue || "TRUE" == *pValue || "1" == *pValue)
			return true;

		if ("false" == *pValue || "False" == *pValue || "FALSE" == *pValue || "0" == *pValue)
			return false;

		return bDefault;
	}

	SPAWN_NPC_PROFILE Parse_NpcProfile_(const _string& strValue)
	{
		if ("Doctor" == strValue)       return SPAWN_NPC_PROFILE::DOCTOR;
		if ("Juveniles" == strValue)    return SPAWN_NPC_PROFILE::JUVENILES;
		if ("Fat" == strValue)          return SPAWN_NPC_PROFILE::FAT;
		if ("Shortpants" == strValue)   return SPAWN_NPC_PROFILE::SHORTPANTS;
		if ("Nurse" == strValue)        return SPAWN_NPC_PROFILE::NURSE;
		if ("Rock" == strValue)         return SPAWN_NPC_PROFILE::ROCK;
		if ("Water" == strValue)        return SPAWN_NPC_PROFILE::WATER;

		return SPAWN_NPC_PROFILE::NONE;
	}

	template<size_t N>
	void Copy_Wide_To_Buffer_(_tchar(&szOut)[N], const _wstring& strValue)
	{
		wcsncpy_s(szOut, strValue.c_str(), _TRUNCATE);
	}

	_wstring To_Wide_(const _string& str)
	{
		if (true == str.empty())
			return L"";

		const int iLength = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
		if (iLength <= 0)
			return _wstring(str.begin(), str.end());

		_wstring strOut;
		strOut.resize(static_cast<size_t>(iLength - 1));
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, strOut.data(), iLength);

		return strOut;
	}

	CGameObject* Resolve_Actor_(EVENT_CONTEXT& tContext, const _wstring& strAlias)
	{
		if (L"Player" == strAlias)
		{
			if (nullptr == tContext.pGameInstance)
				return nullptr;

			const list<CGameObject*>* pList =
				tContext.pGameInstance->Get_ObjectList(ETOUI(LEVEL::GAMEPLAY), LAYER_PLAYER);

			if (nullptr == pList || true == pList->empty())
				return nullptr;

			return pList->front();
		}

		return tContext.Find_Actor(strAlias);
	}

	void Set_CameraPose_(CCamera* pCamera, _fvector vEye, _fvector vAt)
	{
		if (nullptr == pCamera || nullptr == pCamera->Get_Transform())
			return;

		CTransform* pTransform = pCamera->Get_Transform();
		pTransform->Set_State(STATE::POSITION, XMVectorSetW(vEye, 1.f));
		pTransform->LookAt(XMVectorSetW(vAt, 1.f));

		if (CCamera_Free* pFreeCamera = dynamic_cast<CCamera_Free*>(pCamera))
			pFreeCamera->Flush_PipeLine();
		else
			pCamera->Update(0.f);
	}

	void Capture_CameraSnapshot_(EVENT_CONTEXT& tContext, CCamera* pCamera)
	{
		if (nullptr == pCamera || nullptr == pCamera->Get_Transform())
			return;

		CTransform* pTransform = pCamera->Get_Transform();

		const _vector vEye = pTransform->Get_State(STATE::POSITION);
		const _vector vLook = XMVector3Normalize(pTransform->Get_State(STATE::LOOK));
		const _vector vAt = vEye + vLook * 10.f;

		tContext.tCameraSnapshot.bValid = true;
		tContext.tCameraSnapshot.bFollowing = pCamera->Is_Following();
		tContext.tCameraSnapshot.bControlEnabled = pCamera->Is_ControlEnabled();

		XMStoreFloat3(&tContext.tCameraSnapshot.vEye, vEye);
		XMStoreFloat3(&tContext.tCameraSnapshot.vAt, vAt);
	}

	class CEventAction_LockInput final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::LOCK_INPUT; }

		virtual HRESULT Start(EVENT_CONTEXT& tContext) override
		{
			if (nullptr == tContext.pGameInstance)
				return E_FAIL;

			if (false == tContext.bInputLockedByEvent)
			{
				tContext.ePrevInputState = tContext.pGameInstance->Get_InputState();
				tContext.bInputLockedByEvent = true;
			}

			tContext.pGameInstance->Set_InputState(INPUT_STATE::LOCKED);
			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT&, _float) override
		{
			return EVENT_PLAY_STATE::FINISHED;
		}
	};

	class CEventAction_RestoreInput final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::RESTORE_INPUT; }

		virtual HRESULT Start(EVENT_CONTEXT& tContext) override
		{
			if (nullptr == tContext.pGameInstance)
				return E_FAIL;

			if (true == tContext.bInputLockedByEvent)
			{
				tContext.pGameInstance->Set_InputState(tContext.ePrevInputState);
				tContext.bInputLockedByEvent = false;
			}

			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT&, _float) override
		{
			return EVENT_PLAY_STATE::FINISHED;
		}
	};

	class CEventAction_WaitSeconds final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::WAIT_SECONDS; }

		virtual HRESULT Initialize(const EVENT_STEP_DESC& tDesc) override
		{
			if (false == Parse_Float_(tDesc.Params, "Duration", m_fDuration))
				return E_FAIL;

			if (m_fDuration < 0.f)
				m_fDuration = 0.f;

			return S_OK;
		}

		virtual HRESULT Start(EVENT_CONTEXT&) override
		{
			m_fElapsed = 0.f;
			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT&, _float fTimeDelta) override
		{
			if (m_fDuration <= 0.f)
				return EVENT_PLAY_STATE::FINISHED;

			m_fElapsed += fTimeDelta;

			return m_fElapsed >= m_fDuration
				? EVENT_PLAY_STATE::FINISHED
				: EVENT_PLAY_STATE::WAITING;
		}

	private:
		_float m_fDuration = { 0.f };
		_float m_fElapsed = { 0.f };
	};

	class CEventAction_DebugLog final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::DEBUG_LOG; }

		virtual HRESULT Initialize(const EVENT_STEP_DESC& tDesc) override
		{
			m_strText = Read_String_(tDesc.Params, "Text", "");
			return S_OK;
		}

		virtual HRESULT Start(EVENT_CONTEXT&) override
		{
#ifdef _DEBUG
			OutputDebugStringA(("[Event] " + m_strText + "\n").c_str());
#endif
			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT&, _float) override
		{
			return EVENT_PLAY_STATE::FINISHED;
		}

	private:
		_string m_strText;
	};

	class CEventAction_WaitDialogue final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::WAIT_DIALOGUE; }

		virtual HRESULT Start(EVENT_CONTEXT& tContext) override
		{
			if (nullptr == tContext.pLevelGamePlay)
				return E_FAIL;

			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT& tContext, _float) override
		{
			if (nullptr == tContext.pLevelGamePlay)
				return EVENT_PLAY_STATE::FAILED;

			return true == tContext.pLevelGamePlay->Is_Dialogue_Playing()
				? EVENT_PLAY_STATE::WAITING
				: EVENT_PLAY_STATE::FINISHED;
		}
	};

	class CEventAction_MessageKey final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::MESSAGE_KEY; }

		virtual HRESULT Initialize(const EVENT_STEP_DESC& tDesc) override
		{
			m_strKey = To_Wide_(Read_String_(tDesc.Params, "Key", ""));
			m_bWait = Read_Bool_(tDesc.Params, "Wait", true);

			return true == m_strKey.empty() ? E_FAIL : S_OK;
		}

		virtual HRESULT Start(EVENT_CONTEXT& tContext) override
		{
			if (nullptr == tContext.pLevelGamePlay)
				return E_FAIL;

			if (false == tContext.pLevelGamePlay->Start_Dialogue(m_strKey))
				return E_FAIL;

			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT& tContext, _float) override
		{
			if (false == m_bWait)
				return EVENT_PLAY_STATE::FINISHED;

			if (nullptr == tContext.pLevelGamePlay)
				return EVENT_PLAY_STATE::FAILED;

			return true == tContext.pLevelGamePlay->Is_Dialogue_Playing()
				? EVENT_PLAY_STATE::WAITING
				: EVENT_PLAY_STATE::FINISHED;
		}

	private:
		_wstring m_strKey;
		_bool m_bWait = { true };
	};

	class CEventAction_MessageText final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::MESSAGE_TEXT; }

		virtual HRESULT Initialize(const EVENT_STEP_DESC& tDesc) override
		{
			m_strText = To_Wide_(Read_String_(tDesc.Params, "Text", ""));
			m_bWait = Read_Bool_(tDesc.Params, "Wait", true);

			return true == m_strText.empty() ? E_FAIL : S_OK;
		}

		virtual HRESULT Start(EVENT_CONTEXT& tContext) override
		{
			if (nullptr == tContext.pLevelGamePlay)
				return E_FAIL;

			if (false == tContext.pLevelGamePlay->Start_Dialogue_Text(m_strText))
				return E_FAIL;

			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT& tContext, _float) override
		{
			if (false == m_bWait)
				return EVENT_PLAY_STATE::FINISHED;

			if (nullptr == tContext.pLevelGamePlay)
				return EVENT_PLAY_STATE::FAILED;

			return true == tContext.pLevelGamePlay->Is_Dialogue_Playing()
				? EVENT_PLAY_STATE::WAITING
				: EVENT_PLAY_STATE::FINISHED;
		}

	private:
		_wstring m_strText;
		_bool m_bWait = { true };
	};

	class CEventAction_CameraPush final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::CAMERA_PUSH; }

		virtual HRESULT Start(EVENT_CONTEXT& tContext) override
		{
			if (nullptr == tContext.pGameInstance)
				return E_FAIL;

			CCamera* pCamera = tContext.pGameInstance->Get_MainCamera();
			if (nullptr == pCamera)
				return E_FAIL;

			if (false == tContext.tCameraSnapshot.bValid)
				Capture_CameraSnapshot_(tContext, pCamera);

			pCamera->Set_Following(false);
			pCamera->Set_ControlEnabled(false);

			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT&, _float) override
		{
			return EVENT_PLAY_STATE::FINISHED;
		}
	};

	class CEventAction_CameraBlendToActor final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override {
			return
				EVENT_ACTION_KIND::CAMERA_BLEND_TO_ACTOR;
		}

		virtual HRESULT Initialize(const EVENT_STEP_DESC& tDesc) override
		{
			m_strActorAlias = To_Wide_(Read_String_(tDesc.Params, "Actor", "Player"));

			if (false == Parse_Float3_(tDesc.Params, "EyeOffset", m_vEyeOffset))
				m_vEyeOffset = { 0.f, 2.f, -4.f };

			if (false == Parse_Float3_(tDesc.Params, "LookOffset", m_vLookOffset))
				m_vLookOffset = { 0.f, 1.2f, 0.f };

			if (false == Parse_Float_(tDesc.Params, "Duration", m_fDuration))
				m_fDuration = 0.f;

			if (m_fDuration < 0.f)
				m_fDuration = 0.f;

			m_strEase = Read_String_(tDesc.Params, "Ease", "Linear");

			return S_OK;
		}

		virtual HRESULT Start(EVENT_CONTEXT& tContext) override
		{
			if (nullptr == tContext.pGameInstance)
				return E_FAIL;

			CCamera* pCamera = tContext.pGameInstance->Get_MainCamera();
			CGameObject* pActor = Resolve_Actor_(tContext, m_strActorAlias);

			if (nullptr == pCamera || nullptr == pCamera->Get_Transform() ||
				nullptr == pActor || nullptr == pActor->Get_Transform())
				return E_FAIL;

			pCamera->Set_Following(false);
			pCamera->Set_ControlEnabled(false);

			const _vector vCurEye = pCamera->Get_Transform()->Get_State(STATE::POSITION);
			const _vector vCurLook = XMVector3Normalize(pCamera->Get_Transform()->Get_State(STATE::LOOK));
			const _vector vCurAt = vCurEye + vCurLook * 10.f;

			XMStoreFloat3(&m_vStartEye, vCurEye);
			XMStoreFloat3(&m_vStartAt, vCurAt);

			const _vector vActorPos = pActor->Get_Transform()->Get_State(STATE::POSITION);
			const _vector vTargetEye = vActorPos + XMLoadFloat3(&m_vEyeOffset);
			const _vector vTargetAt = vActorPos + XMLoadFloat3(&m_vLookOffset);

			XMStoreFloat3(&m_vTargetEye, vTargetEye);
			XMStoreFloat3(&m_vTargetAt, vTargetAt);

			m_fElapsed = 0.f;

			if (m_fDuration <= 0.f)
				Set_CameraPose_(pCamera, vTargetEye, vTargetAt);

			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT& tContext, _float fTimeDelta) override
		{
			if (nullptr == tContext.pGameInstance)
				return EVENT_PLAY_STATE::FAILED;

			CCamera* pCamera = tContext.pGameInstance->Get_MainCamera();
			if (nullptr == pCamera)
				return EVENT_PLAY_STATE::FAILED;

			if (m_fDuration <= 0.f)
				return EVENT_PLAY_STATE::FINISHED;

			m_fElapsed += fTimeDelta;

			_float fRatio = m_fElapsed / m_fDuration;
			if (fRatio > 1.f)
				fRatio = 1.f;

			if ("SmoothStep" == m_strEase || "smoothstep" == m_strEase)
				fRatio = fRatio * fRatio * (3.f - 2.f * fRatio);

			const _vector vStartEye = XMLoadFloat3(&m_vStartEye);
			const _vector vStartAt = XMLoadFloat3(&m_vStartAt);
			const _vector vTargetEye = XMLoadFloat3(&m_vTargetEye);
			const _vector vTargetAt = XMLoadFloat3(&m_vTargetAt);

			Set_CameraPose_(
				pCamera,
				XMVectorLerp(vStartEye, vTargetEye, fRatio),
				XMVectorLerp(vStartAt, vTargetAt, fRatio));

			return m_fElapsed >= m_fDuration
				? EVENT_PLAY_STATE::FINISHED
				: EVENT_PLAY_STATE::WAITING;
		}

	private:
		_wstring m_strActorAlias = { L"Player" };
		_float3 m_vEyeOffset = { 0.f, 2.f, -4.f };
		_float3 m_vLookOffset = { 0.f, 1.2f, 0.f };
		_float m_fDuration = { 0.f };
		_float m_fElapsed = { 0.f };
		_string m_strEase = { "Linear" };

		_float3 m_vStartEye = {};
		_float3 m_vStartAt = {};
		_float3 m_vTargetEye = {};
		_float3 m_vTargetAt = {};
	};

	class CEventAction_CameraPop final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::CAMERA_POP; }

		virtual HRESULT Initialize(const EVENT_STEP_DESC& tDesc) override
		{
			if (false == Parse_Float_(tDesc.Params, "Duration", m_fDuration))
				m_fDuration = 0.f;

			if (m_fDuration < 0.f)
				m_fDuration = 0.f;

			return S_OK;
		}

		virtual HRESULT Start(EVENT_CONTEXT& tContext) override
		{
			if (nullptr == tContext.pGameInstance || false == tContext.tCameraSnapshot.bValid)
				return E_FAIL;

			CCamera* pCamera = tContext.pGameInstance->Get_MainCamera();
			if (nullptr == pCamera || nullptr == pCamera->Get_Transform())
				return E_FAIL;

			const _vector vCurEye = pCamera->Get_Transform()->Get_State(STATE::POSITION);
			const _vector vCurLook = XMVector3Normalize(pCamera->Get_Transform()->Get_State(STATE::LOOK));
			const _vector vCurAt = vCurEye + vCurLook * 10.f;

			XMStoreFloat3(&m_vStartEye, vCurEye);
			XMStoreFloat3(&m_vStartAt, vCurAt);

			m_fElapsed = 0.f;

			if (m_fDuration <= 0.f)
				Restore(tContext, pCamera);

			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT& tContext, _float fTimeDelta) override
		{
			if (false == tContext.tCameraSnapshot.bValid)
				return EVENT_PLAY_STATE::FAILED;

			CCamera* pCamera = tContext.pGameInstance ? tContext.pGameInstance->Get_MainCamera() : nullptr;
			if (nullptr == pCamera)
				return EVENT_PLAY_STATE::FAILED;

			if (m_fDuration <= 0.f)
				return EVENT_PLAY_STATE::FINISHED;

			m_fElapsed += fTimeDelta;

			_float fRatio = m_fElapsed / m_fDuration;
			if (fRatio > 1.f)
				fRatio = 1.f;

			const _vector vStartEye = XMLoadFloat3(&m_vStartEye);
			const _vector vStartAt = XMLoadFloat3(&m_vStartAt);
			const _vector vTargetEye = XMLoadFloat3(&tContext.tCameraSnapshot.vEye);
			const _vector vTargetAt = XMLoadFloat3(&tContext.tCameraSnapshot.vAt);

			Set_CameraPose_(
				pCamera,
				XMVectorLerp(vStartEye, vTargetEye, fRatio),
				XMVectorLerp(vStartAt, vTargetAt, fRatio));

			if (m_fElapsed >= m_fDuration)
			{
				Restore(tContext, pCamera);
				return EVENT_PLAY_STATE::FINISHED;
			}

			return EVENT_PLAY_STATE::WAITING;
		}

	private:
		void Restore(EVENT_CONTEXT& tContext, CCamera* pCamera)
		{
			Set_CameraPose_(
				pCamera,
				XMLoadFloat3(&tContext.tCameraSnapshot.vEye),
				XMLoadFloat3(&tContext.tCameraSnapshot.vAt));

			pCamera->Set_Following(tContext.tCameraSnapshot.bFollowing);
			pCamera->Set_ControlEnabled(tContext.tCameraSnapshot.bControlEnabled);

			tContext.tCameraSnapshot = {};
		}

	private:
		_float m_fDuration = { 0.f };
		_float m_fElapsed = { 0.f };
		_float3 m_vStartEye = {};
		_float3 m_vStartAt = {};
	};

	class CEventAction_SpawnNPC final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::SPAWN_NPC; }

		virtual HRESULT Initialize(const EVENT_STEP_DESC& tDesc) override
		{
			m_eNpcProfile = Parse_NpcProfile_(Read_String_(tDesc.Params, "NPCProfile", ""));
			if (SPAWN_NPC_PROFILE::NONE == m_eNpcProfile)
				return E_FAIL;

			if (false == Parse_Float3_(tDesc.Params, "Position", m_vPosition))
				return E_FAIL;

			if (false == Parse_Float_(tDesc.Params, "RotationY", m_fRotationY))
				m_fRotationY = 0.f;

			m_strDialogueKey = To_Wide_(Read_String_(tDesc.Params, "DialogueKey", ""));
			m_strEventSequenceID = To_Wide_(Read_String_(tDesc.Params, "EventSequenceID", ""));
			m_strAlias = To_Wide_(Read_String_(tDesc.Params, "Alias", ""));
			m_bDestroyOnCancel = Read_Bool_(tDesc.Params, "DestroyOnCancel", true);

			return S_OK;
		}

		virtual HRESULT Start(EVENT_CONTEXT& tContext) override
		{
			EVENT_NPC_SPAWN_DESC SpawnDesc{};
			SpawnDesc.eNpcProfile = m_eNpcProfile;
			SpawnDesc.vPosition = m_vPosition;
			SpawnDesc.fRotationY = m_fRotationY;

			Copy_Wide_To_Buffer_(SpawnDesc.szDialogueKey, m_strDialogueKey);
			Copy_Wide_To_Buffer_(SpawnDesc.szEventSequenceID, m_strEventSequenceID);

			CActor_NPC* pNPC = { nullptr };
			if (FAILED(CSpawn_Manager::GetInstance()->Spawn_NPC_Immediate(SpawnDesc, &pNPC)))
				return E_FAIL;

			m_pSpawnedActor = pNPC;

			if (false == m_strAlias.empty())
				tContext.Bind_Actor(m_strAlias, pNPC);

			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT&, _float) override
		{
			return EVENT_PLAY_STATE::FINISHED;
		}

		virtual void Cancel(EVENT_CONTEXT&) override
		{
			if (true == m_bDestroyOnCancel &&
				nullptr != m_pSpawnedActor &&
				false == m_pSpawnedActor->Is_Dead())
			{
				m_pSpawnedActor->Set_Dead();
			}

			m_pSpawnedActor = nullptr;
		}

	private:
		SPAWN_NPC_PROFILE m_eNpcProfile = { SPAWN_NPC_PROFILE::NONE };
		_float3 m_vPosition = {};
		_float m_fRotationY = { 0.f };

		_wstring m_strDialogueKey;
		_wstring m_strEventSequenceID;
		_wstring m_strAlias;

		_bool m_bDestroyOnCancel = { true };
		CActor_NPC* m_pSpawnedActor = { nullptr }; // weak
	};

	class CEventAction_DespawnActor final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::DESPAWN_ACTOR; }

		virtual HRESULT Initialize(const EVENT_STEP_DESC& tDesc) override
		{
			m_strActorAlias = To_Wide_(Read_String_(tDesc.Params, "Actor", ""));
			return true == m_strActorAlias.empty() ? E_FAIL : S_OK;
		}

		virtual HRESULT Start(EVENT_CONTEXT& tContext) override
		{
			CGameObject* pActor = Resolve_Actor_(tContext, m_strActorAlias);
			if (nullptr == pActor || true == pActor->Is_Dead())
				return E_FAIL;

			pActor->Set_Dead();

			if (L"Player" != m_strActorAlias &&
				L"Caller" != m_strActorAlias &&
				L"$caller" != m_strActorAlias &&
				L"Target" != m_strActorAlias &&
				L"$target" != m_strActorAlias)
			{
				tContext.Bind_Actor(m_strActorAlias, nullptr);
			}

			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT&, _float) override
		{
			return EVENT_PLAY_STATE::FINISHED;
		}

	private:
		_wstring m_strActorAlias;
	};
}

CEventAction* CEventAction::Create_Action(const EVENT_STEP_DESC& tDesc)
{
	CEventAction* pAction = { nullptr };

	switch (tDesc.eKind)
	{
	case EVENT_ACTION_KIND::LOCK_INPUT:
		pAction = new CEventAction_LockInput;
		break;

	case EVENT_ACTION_KIND::RESTORE_INPUT:
		pAction = new CEventAction_RestoreInput;
		break;

	case EVENT_ACTION_KIND::WAIT_SECONDS:
		pAction = new CEventAction_WaitSeconds;
		break;

	case EVENT_ACTION_KIND::DEBUG_LOG:
		pAction = new CEventAction_DebugLog;
		break;

	case EVENT_ACTION_KIND::WAIT_DIALOGUE:
		pAction = new CEventAction_WaitDialogue;
		break;

	case EVENT_ACTION_KIND::MESSAGE_KEY:
		pAction = new CEventAction_MessageKey;
		break;

	case EVENT_ACTION_KIND::MESSAGE_TEXT:
		pAction = new CEventAction_MessageText;
		break;

	case EVENT_ACTION_KIND::CAMERA_PUSH:
		pAction = new CEventAction_CameraPush;
		break;

	case EVENT_ACTION_KIND::CAMERA_BLEND_TO_ACTOR:
		pAction = new CEventAction_CameraBlendToActor;
		break;

	case EVENT_ACTION_KIND::CAMERA_POP:
		pAction = new CEventAction_CameraPop;
		break;

	case EVENT_ACTION_KIND::SPAWN_NPC:
		pAction = new CEventAction_SpawnNPC;
		break;

	case EVENT_ACTION_KIND::DESPAWN_ACTOR:
		pAction = new CEventAction_DespawnActor;
		break;

	default:
		return nullptr;
	}

	if (FAILED(pAction->Initialize(tDesc)))
	{
		Safe_Release(pAction);
		return nullptr;
	}

	return pAction;
}

void CEventAction::Free()
{
	__super::Free();
}