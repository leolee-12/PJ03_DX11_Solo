#include "EventAction.h"
#include "Level_GamePlay.h"
#include "Camera_Free.h"
#include "Spawn_Manager.h"
#include "Actor_NPC.h"
#include "Battle_AnimDef.h"

#include "GameInstance.h"

#include <sstream>
#include <cmath>

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

	_bool Parse_UInt_(const unordered_map<_string, _string>& Params, const _string& strKey, _uint& iOut)
	{
		const _string* pValue = Find_Param_(Params, strKey);
		if (nullptr == pValue)
			return false;

		try
		{
			iOut = static_cast<_uint>(std::stoul(*pValue));
		}
		catch (...)
		{
			return false;
		}

		return true;
	}

	_string Normalize_Token_(const _string& strValue)
	{
		_string strOut;
		strOut.reserve(strValue.size());

		for (_char ch : strValue)
		{
			if ('_' == ch || '-' == ch || ' ' == ch || '\t' == ch)
				continue;

			strOut.push_back(static_cast<_char>(::toupper(static_cast<unsigned char>(ch))));
		}

		return strOut;
	}

	ENVIRONMENT_TYPE Parse_EnvironmentType_(const _string& strValue)
	{
		const _string strKey = Normalize_Token_(strValue);

		if ("PLAIN" == strKey)  return ENVIRONMENT_TYPE::PLAIN;
		if ("GRASS" == strKey)  return ENVIRONMENT_TYPE::GRASS;
		if ("FOREST" == strKey) return ENVIRONMENT_TYPE::FOREST;
		if ("CAVE" == strKey)   return ENVIRONMENT_TYPE::CAVE;
		if ("WATER" == strKey)  return ENVIRONMENT_TYPE::WATER;
		if ("DESERT" == strKey) return ENVIRONMENT_TYPE::DESERT;
		if ("SNOW" == strKey)   return ENVIRONMENT_TYPE::SNOW;
		if ("URBAN" == strKey)  return ENVIRONMENT_TYPE::URBAN;

		return ENVIRONMENT_TYPE::GRASS;
	}

	BATTLE_RULE Parse_BattleRule_(const _string& strValue)
	{
		const _string strKey = Normalize_Token_(strValue);

		if ("WILDSINGLE" == strKey)     return BATTLE_RULE::WILD_SINGLE;
		if ("TRAINERSINGLE" == strKey)  return BATTLE_RULE::TRAINER_SINGLE;
		if ("TRAINERDOUBLE" == strKey)  return BATTLE_RULE::TRAINER_DOUBLE;
		if ("CUTSCENE" == strKey)       return BATTLE_RULE::CUTSCENE;
		if ("TUTORIAL" == strKey)       return BATTLE_RULE::TUTORIAL;

		return BATTLE_RULE::TRAINER_SINGLE;
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

	ANIM_KIND Parse_AnimKind_(const _string& strValue)
	{
		if ("IDLE" == strValue)            return ANIM_KIND::IDLE;
		if ("WALK" == strValue)            return ANIM_KIND::WALK;
		if ("RUN" == strValue)             return ANIM_KIND::RUN;
		if ("TALK" == strValue)            return ANIM_KIND::TALK;
		if ("HURT" == strValue)            return ANIM_KIND::HURT;
		if ("FAINT" == strValue)           return ANIM_KIND::FAINT;
		if ("INTRO" == strValue)           return ANIM_KIND::INTRO;
		if ("FOCUS" == strValue)           return ANIM_KIND::FOCUS;
		if ("ORDER" == strValue)           return ANIM_KIND::ORDER;
		if ("THROW" == strValue)           return ANIM_KIND::THROW;
		if ("SWITCH" == strValue)          return ANIM_KIND::SWITCH;
		if ("ATTACK_PHYSICAL" == strValue) return ANIM_KIND::ATTACK_PHYSICAL;
		if ("ATTACK_SPECIAL" == strValue)  return ANIM_KIND::ATTACK_SPECIAL;
		if ("IDLE_1" == strValue)          return ANIM_KIND::IDLE_1;
		if ("IDLE_2" == strValue)          return ANIM_KIND::IDLE_2;
		if ("IDLE_3" == strValue)          return ANIM_KIND::IDLE_3;
		if ("EVENT_1" == strValue)         return ANIM_KIND::EVENT_1;
		if ("EVENT_2" == strValue)         return ANIM_KIND::EVENT_2;
		if ("EVENT_3" == strValue)         return ANIM_KIND::EVENT_3;

		return ANIM_KIND::END;
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

#ifdef _DEBUG
	_string To_Narrow_Debug_(const _wstring& str)
	{
		if (true == str.empty())
			return "";

		const int iLength = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, nullptr, 0, nullptr, nullptr);
		if (iLength <= 0)
			return _string(str.begin(), str.end());

		_string strOut;
		strOut.resize(static_cast<size_t>(iLength - 1));
		WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &strOut[0], iLength, nullptr, nullptr);
		return strOut;
	}

	void Debug_EventActor_(const _string& strText)
	{
		OutputDebugStringA(("[Event Actor] " + strText + "\n").c_str());
	}
#endif

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

#ifdef _DEBUG
				OutputDebugStringA(("[Event] LockInput saved PrevInputState=" +
					std::to_string(static_cast<_int>(tContext.ePrevInputState)) + "\n").c_str());
#endif
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
#ifdef _DEBUG
				OutputDebugStringA(("[Event] RestoreInput restoring to=" +
					std::to_string(static_cast<_int>(tContext.ePrevInputState)) + "\n").c_str());
#endif
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

	class CEventAction_CameraSetPose final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::CAMERA_SET_POSE; }

		virtual HRESULT Initialize(const EVENT_STEP_DESC& tDesc) override
		{
			if (false == Parse_Float3_(tDesc.Params, "Eye", m_vEye))
				return E_FAIL;

			if (false == Parse_Float3_(tDesc.Params, "LookAt", m_vAt))
				return E_FAIL;

			return S_OK;
		}

		virtual HRESULT Start(EVENT_CONTEXT& tContext) override
		{
			if (nullptr == tContext.pGameInstance)
				return E_FAIL;

			CCamera* pCamera = tContext.pGameInstance->Get_MainCamera();
			if (nullptr == pCamera)
				return E_FAIL;

			pCamera->Set_Following(false);
			pCamera->Set_ControlEnabled(false);

			Set_CameraPose_(
				pCamera,
				XMLoadFloat3(&m_vEye),
				XMLoadFloat3(&m_vAt));

			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT&, _float) override
		{
			return EVENT_PLAY_STATE::FINISHED;
		}

	private:
		_float3 m_vEye = {};
		_float3 m_vAt = {};
	};

	class CEventAction_CameraMoveTo final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::CAMERA_MOVE_TO; }

		virtual HRESULT Initialize(const EVENT_STEP_DESC& tDesc) override
		{
			if (false == Parse_Float3_(tDesc.Params, "Eye", m_vTargetEye))
				return E_FAIL;

			if (false == Parse_Float3_(tDesc.Params, "LookAt", m_vTargetAt))
				return E_FAIL;

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
			if (nullptr == pCamera || nullptr == pCamera->Get_Transform())
				return E_FAIL;

			pCamera->Set_Following(false);
			pCamera->Set_ControlEnabled(false);

			const _vector vStartEye = pCamera->Get_Transform()->Get_State(STATE::POSITION);
			const _vector vStartLook = XMVector3Normalize(pCamera->Get_Transform()->Get_State(STATE::LOOK));
			const _vector vStartAt = vStartEye + vStartLook * 10.f;

			XMStoreFloat3(&m_vStartEye, vStartEye);
			XMStoreFloat3(&m_vStartAt, vStartAt);

			m_fElapsed = 0.f;

			if (m_fDuration <= 0.f)
			{
				Set_CameraPose_(
					pCamera,
					XMLoadFloat3(&m_vTargetEye),
					XMLoadFloat3(&m_vTargetAt));
			}

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

			Set_CameraPose_(
				pCamera,
				XMVectorLerp(XMLoadFloat3(&m_vStartEye), XMLoadFloat3(&m_vTargetEye), fRatio),
				XMVectorLerp(XMLoadFloat3(&m_vStartAt), XMLoadFloat3(&m_vTargetAt), fRatio));

			return m_fElapsed >= m_fDuration
				? EVENT_PLAY_STATE::FINISHED
				: EVENT_PLAY_STATE::WAITING;
		}

	private:
		_float3 m_vStartEye = {};
		_float3 m_vStartAt = {};
		_float3 m_vTargetEye = {};
		_float3 m_vTargetAt = {};
		_float m_fDuration = { 0.f };
		_float m_fElapsed = { 0.f };
		_string m_strEase = { "Linear" };
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

	class CEventAction_BindActorBySpawnID final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override {
			return
				EVENT_ACTION_KIND::BIND_ACTOR_BY_SPAWN_ID;
		}

		virtual HRESULT Initialize(const EVENT_STEP_DESC& tDesc) override
		{
			if (false == Parse_UInt_(tDesc.Params, "SpawnID", m_iSpawnID))
				return E_FAIL;

			m_strAlias = To_Wide_(Read_String_(tDesc.Params, "Alias", ""));
			return true == m_strAlias.empty() ? E_FAIL : S_OK;
		}

		virtual HRESULT Start(EVENT_CONTEXT& tContext) override
		{
			if (nullptr == tContext.pGameInstance)
				return E_FAIL;

			const list<CGameObject*>* pList =
				tContext.pGameInstance->Get_ObjectList(ETOUI(LEVEL::GAMEPLAY), LAYER_NPC);

			if (nullptr == pList)
				return E_FAIL;

			for (CGameObject* pObject : *pList)
			{
				if (nullptr == pObject || true == pObject->Is_Dead())
					continue;

				CActor_NPC* pNPC = dynamic_cast<CActor_NPC*>(pObject);
				if (nullptr == pNPC)
					continue;

				if (m_iSpawnID != pNPC->Get_SpawnRectID())
					continue;

				tContext.Bind_Actor(m_strAlias, pNPC);

#ifdef _DEBUG
				{
					std::ostringstream oss;
					oss << "BindActorBySpawnID success SpawnID=" << m_iSpawnID
						<< " Alias=" << To_Narrow_Debug_(m_strAlias)
						<< " NPC=" << pNPC;
					Debug_EventActor_(oss.str());
				}
#endif

				return S_OK;
			}

#ifdef _DEBUG
			OutputDebugStringA("[Event Warn] BindActorBySpawnID: SpawnID not found\n");
#endif

			return E_FAIL;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT&, _float) override
		{
			return EVENT_PLAY_STATE::FINISHED;
		}

	private:
		_uint m_iSpawnID = { 0 };
		_wstring m_strAlias;
	};

	class CEventAction_ActorFace final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::ACTOR_FACE; }

		virtual HRESULT Initialize(const EVENT_STEP_DESC& tDesc) override
		{
			m_strActorAlias = To_Wide_(Read_String_(tDesc.Params, "Actor", "Target"));
			m_strTargetAlias = To_Wide_(Read_String_(tDesc.Params, "Target", "Caller"));
			m_bWait = Read_Bool_(tDesc.Params, "Wait", false);
			return S_OK;
		}

		virtual HRESULT Start(EVENT_CONTEXT& tContext) override
		{
			CGameObject* pActor = Resolve_Actor_(tContext, m_strActorAlias);
			CGameObject* pTarget = Resolve_Actor_(tContext, m_strTargetAlias);

			if (nullptr == pActor || nullptr == pTarget ||
				true == pActor->Is_Dead() || true == pTarget->Is_Dead())
			{
#ifdef _DEBUG
				OutputDebugStringA("[Event Warn] ActorFace: alias unresolved/dead "
					"(Actor/Target nullptr or Is_Dead)\n");
#endif
				return E_FAIL;
			}

			CActor_NPC* pNPC = dynamic_cast<CActor_NPC*>(pActor);
			if (nullptr == pNPC)
			{
#ifdef _DEBUG
				OutputDebugStringA("[Event Warn] ActorFace: Actor is not CActor_NPC\n");
#endif
				return E_FAIL;
			}

			if (nullptr == pTarget->Get_Transform())
			{
#ifdef _DEBUG
				OutputDebugStringA("[Event Warn] ActorFace: Target has no Transform\n");
#endif
				return E_FAIL;
			}

			const _vector vTargetPos = pTarget->Get_Transform()->Get_State(STATE::POSITION);
			pNPC->Face_To(vTargetPos);

#ifdef _DEBUG
			Debug_EventActor_("ActorFace Start Actor=" + To_Narrow_Debug_(m_strActorAlias) +
				" Target=" + To_Narrow_Debug_(m_strTargetAlias) +
				" Wait=" + std::to_string(m_bWait));
#endif

			m_pTrackedNPC = pNPC;
			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT&, _float) override
		{
			if (false == m_bWait)
				return EVENT_PLAY_STATE::FINISHED;

			if (nullptr == m_pTrackedNPC || true == m_pTrackedNPC->Is_Dead())
				return EVENT_PLAY_STATE::FINISHED;

			return true == m_pTrackedNPC->Is_FaceTurnActive()
				? EVENT_PLAY_STATE::WAITING
				: EVENT_PLAY_STATE::FINISHED;
		}

	private:
		_wstring m_strActorAlias;
		_wstring m_strTargetAlias;
		_bool m_bWait = { false };
		CActor_NPC* m_pTrackedNPC = { nullptr }; // weak
	};

	class CEventAction_ActorFaceYaw final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::ACTOR_FACE_YAW; }

		virtual HRESULT Initialize(const EVENT_STEP_DESC& tDesc) override
		{
			m_strActorAlias = To_Wide_(Read_String_(tDesc.Params, "Actor", "Target"));
			if (false == Parse_Float_(tDesc.Params, "Yaw", m_fYaw))
				return E_FAIL;

			m_bWait = Read_Bool_(tDesc.Params, "Wait", false);
			return S_OK;
		}

		virtual HRESULT Start(EVENT_CONTEXT& tContext) override
		{
			CGameObject* pActor = Resolve_Actor_(tContext, m_strActorAlias);
			if (nullptr == pActor || true == pActor->Is_Dead() || nullptr == pActor->Get_Transform())
				return E_FAIL;

			CActor_NPC* pNPC = dynamic_cast<CActor_NPC*>(pActor);
			if (nullptr == pNPC)
				return E_FAIL;

			const _vector vPos = pActor->Get_Transform()->Get_State(STATE::POSITION);
			const _vector vDir = XMVectorSet(sinf(m_fYaw), 0.f, cosf(m_fYaw), 0.f);
			pNPC->Face_To(XMVectorAdd(vPos, XMVectorScale(vDir, 10.f)));

#ifdef _DEBUG
			Debug_EventActor_("ActorFaceYaw Start Actor=" + To_Narrow_Debug_(m_strActorAlias) +
				" Yaw=" + std::to_string(m_fYaw) +
				" Wait=" + std::to_string(m_bWait));
#endif

			m_pTrackedNPC = pNPC;
			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT&, _float) override
		{
			if (false == m_bWait)
				return EVENT_PLAY_STATE::FINISHED;

			if (nullptr == m_pTrackedNPC || true == m_pTrackedNPC->Is_Dead())
				return EVENT_PLAY_STATE::FINISHED;

			return true == m_pTrackedNPC->Is_FaceTurnActive()
				? EVENT_PLAY_STATE::WAITING
				: EVENT_PLAY_STATE::FINISHED;
		}

	private:
		_wstring m_strActorAlias;
		_float m_fYaw = { 0.f };
		_bool m_bWait = { false };
		CActor_NPC* m_pTrackedNPC = { nullptr };
	};

	class CEventAction_ActorFaceCamera final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::ACTOR_FACE_CAMERA; }

		virtual HRESULT Initialize(const EVENT_STEP_DESC& tDesc) override
		{
			m_strActorAlias = To_Wide_(Read_String_(tDesc.Params, "Actor", "Target"));
			m_bWait = Read_Bool_(tDesc.Params, "Wait", false);
			return S_OK;
		}

		virtual HRESULT Start(EVENT_CONTEXT& tContext) override
		{
			if (nullptr == tContext.pGameInstance)
				return E_FAIL;

			CGameObject* pActor = Resolve_Actor_(tContext, m_strActorAlias);
			CCamera* pCamera = tContext.pGameInstance->Get_MainCamera();

			if (nullptr == pActor || true == pActor->Is_Dead() || nullptr == pCamera || nullptr ==
				pCamera->Get_Transform())
				return E_FAIL;

			CActor_NPC* pNPC = dynamic_cast<CActor_NPC*>(pActor);
			if (nullptr == pNPC)
				return E_FAIL;

			pNPC->Face_To(pCamera->Get_Transform()->Get_State(STATE::POSITION));

#ifdef _DEBUG
			Debug_EventActor_("ActorFaceCamera Start Actor=" + To_Narrow_Debug_(m_strActorAlias) +
				" Wait=" + std::to_string(m_bWait));
#endif

			m_pTrackedNPC = pNPC;
			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT&, _float) override
		{
			if (false == m_bWait)
				return EVENT_PLAY_STATE::FINISHED;

			if (nullptr == m_pTrackedNPC || true == m_pTrackedNPC->Is_Dead())
				return EVENT_PLAY_STATE::FINISHED;

			return true == m_pTrackedNPC->Is_FaceTurnActive()
				? EVENT_PLAY_STATE::WAITING
				: EVENT_PLAY_STATE::FINISHED;
		}

	private:
		_wstring m_strActorAlias;
		_bool m_bWait = { false };
		CActor_NPC* m_pTrackedNPC = { nullptr };
	};

	class CEventAction_ActorSetAnim final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::ACTOR_SET_ANIM; }

		virtual HRESULT Initialize(const EVENT_STEP_DESC& tDesc) override
		{
			m_strActorAlias = To_Wide_(Read_String_(tDesc.Params, "Actor", "Target"));
			m_eAnimKind = Parse_AnimKind_(Read_String_(tDesc.Params, "AnimKind", ""));
			m_bLoop = Read_Bool_(tDesc.Params, "Loop", true);

			if (false == Parse_Float_(tDesc.Params, "Blend", m_fBlend))
				m_fBlend = 0.2f;

			if (ANIM_KIND::END == m_eAnimKind)
				return E_FAIL;

			return S_OK;
		}

		virtual HRESULT Start(EVENT_CONTEXT& tContext) override
		{
			CGameObject* pActor = Resolve_Actor_(tContext, m_strActorAlias);
			if (nullptr == pActor || true == pActor->Is_Dead())
			{
#ifdef _DEBUG
				OutputDebugStringA("[Event Warn] ActorSetAnim: alias unresolved or Is_Dead\n");
#endif
				return E_FAIL;
			}

			CActor* pActorBase = dynamic_cast<CActor*>(pActor);
			if (nullptr == pActorBase || nullptr == pActorBase->Get_Body())
			{
#ifdef _DEBUG
				OutputDebugStringA("[Event Warn] ActorSetAnim: dynamic_cast<CActor*> or Get_Body() nullptr\n");
#endif
				return E_FAIL;
			}

			CBody* pBody = pActorBase->Get_Body();
			const WNameID iModelTag = pBody->Get_ModelProtoTag();
			const _uint iIndex = BattleAnim::Find_AnimIndex(iModelTag, m_eAnimKind);
			const _bool bApplied = pBody->Set_Anim(iIndex, m_bLoop, m_fBlend);

#ifdef _DEBUG
			{
				std::ostringstream oss;
				oss << "ActorSetAnim Actor=" << To_Narrow_Debug_(m_strActorAlias)
					<< " ModelTag=" << WtoS(WNameRegistry::Lookup(iModelTag))
					<< " AnimKind=" << static_cast<_uint>(m_eAnimKind)
					<< " AnimIndex=" << iIndex
					<< " Loop=" << m_bLoop
					<< " Blend=" << m_fBlend
					<< " Applied=" << bApplied;
				Debug_EventActor_(oss.str());
			}
#endif

			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT&, _float) override
		{
			return EVENT_PLAY_STATE::FINISHED;
		}

	private:
		_wstring m_strActorAlias;
		ANIM_KIND m_eAnimKind = { ANIM_KIND::END };
		_bool m_bLoop = { true };
		_float m_fBlend = { 0.2f };
	};

	class CEventAction_ActorMoveTo final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::ACTOR_MOVE_TO; }

		virtual HRESULT Initialize(const EVENT_STEP_DESC& tDesc) override
		{
			m_strActorAlias = To_Wide_(Read_String_(tDesc.Params, "Actor", "Target"));

			if (false == Parse_Float3_(tDesc.Params, "Position", m_vTargetPos))
				return E_FAIL;

			if (false == Parse_Float_(tDesc.Params, "Speed", m_fSpeed))
				m_fSpeed = 3.f;

			if (false == Parse_Float_(tDesc.Params, "ArrivalRadius", m_fArrivalRadius))
				m_fArrivalRadius = 0.15f;

			m_eWalkKind = Parse_AnimKind_(Read_String_(tDesc.Params, "WalkAnim", "WALK"));
			m_eEndKind = Parse_AnimKind_(Read_String_(tDesc.Params, "EndAnim", "IDLE"));

			if (false == Parse_Float_(tDesc.Params, "Blend", m_fBlend))
				m_fBlend = 0.2f;

			return S_OK;
		}

		virtual HRESULT Start(EVENT_CONTEXT& tContext) override
		{
			CGameObject* pActor = Resolve_Actor_(tContext, m_strActorAlias);
			if (nullptr == pActor || true == pActor->Is_Dead() || nullptr == pActor->Get_Transform())
			{
#ifdef _DEBUG
				OutputDebugStringA("[Event Warn] ActorMoveTo: alias unresolved/dead or no Transform\n");
#endif
				return E_FAIL;
			}

			m_pTrackedActor = dynamic_cast<CActor*>(pActor);
			if (nullptr == m_pTrackedActor)
			{
#ifdef _DEBUG
				OutputDebugStringA("[Event Warn] ActorMoveTo: Actor is not CActor\n");
#endif
				return E_FAIL;
			}

			if (ANIM_KIND::END != m_eWalkKind && nullptr != m_pTrackedActor->Get_Body())
			{
				CBody* pBody = m_pTrackedActor->Get_Body();
				const _uint iIndex = BattleAnim::Find_AnimIndex(pBody->Get_ModelProtoTag(), m_eWalkKind);
				pBody->Set_Anim(iIndex, true, m_fBlend);
			}

			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT&, _float fTimeDelta) override
		{
			if (nullptr == m_pTrackedActor || true == m_pTrackedActor->Is_Dead())
				return EVENT_PLAY_STATE::FAILED;

			CTransform* pTransform = m_pTrackedActor->Get_Transform();
			if (nullptr == pTransform)
				return EVENT_PLAY_STATE::FAILED;

			const _vector vCurPos = pTransform->Get_State(STATE::POSITION);
			const _vector vTargetPos = XMLoadFloat3(&m_vTargetPos);

			_float3 vDeltaXZ{};
			XMStoreFloat3(&vDeltaXZ, XMVectorSubtract(vTargetPos, vCurPos));
			vDeltaXZ.y = 0.f;

			const _float fDistance = sqrtf(vDeltaXZ.x * vDeltaXZ.x + vDeltaXZ.z * vDeltaXZ.z);

			if (fDistance <= m_fArrivalRadius)
			{
				pTransform->Set_State(STATE::POSITION, XMVectorSetW(vTargetPos, 1.f));

				if (ANIM_KIND::END != m_eEndKind && nullptr != m_pTrackedActor->Get_Body())
				{
					CBody* pBody = m_pTrackedActor->Get_Body();
					const _uint iIndex = BattleAnim::Find_AnimIndex(pBody->Get_ModelProtoTag(),
						m_eEndKind);
					pBody->Set_Anim(iIndex, true, m_fBlend);
				}

				return EVENT_PLAY_STATE::FINISHED;
			}

			const _float fInvDistance = 1.f / fDistance;
			const _float3 vDirF3 = { vDeltaXZ.x * fInvDistance, 0.f, vDeltaXZ.z * fInvDistance };
			const _vector vDir = XMLoadFloat3(&vDirF3);

			const _float fStepRaw = m_fSpeed * fTimeDelta;
			const _float fStep = fStepRaw < fDistance ? fStepRaw : fDistance;
			const _vector vNewPos = XMVectorAdd(vCurPos, XMVectorScale(vDir, fStep));

			pTransform->Set_State(STATE::POSITION, XMVectorSetW(vNewPos, 1.f));
			pTransform->LookAt(XMVectorSetW(XMVectorAdd(vNewPos, vDir), 1.f));

			return EVENT_PLAY_STATE::WAITING;
		}

	private:
		_wstring m_strActorAlias;
		_float3 m_vTargetPos = {};
		_float m_fSpeed = { 3.f };
		_float m_fArrivalRadius = { 0.15f };
		ANIM_KIND m_eWalkKind = { ANIM_KIND::WALK };
		ANIM_KIND m_eEndKind = { ANIM_KIND::IDLE };
		_float m_fBlend = { 0.2f };
		CActor* m_pTrackedActor = { nullptr }; // weak
	};

	class CEventAction_RequestBattle final : public CEventAction
	{
	public:
		virtual EVENT_ACTION_KIND Get_Kind() const override { return EVENT_ACTION_KIND::REQUEST_BATTLE; }

		virtual HRESULT Initialize(const EVENT_STEP_DESC& tDesc) override
		{
			m_tEnv = {};
			m_tEnv.eEnvironment = Parse_EnvironmentType_(Read_String_(tDesc.Params, "Environment", "GRASS"));
			m_tEnv.eRule = Parse_BattleRule_(Read_String_(tDesc.Params, "Rule", "TRAINER_SINGLE"));

			if (false == Parse_UInt_(tDesc.Params, "TrainerID", m_tEnv.iOpponentTrainerID))
				Parse_UInt_(tDesc.Params, "OpponentTrainerID", m_tEnv.iOpponentTrainerID);

			Parse_UInt_(tDesc.Params, "BGResourceID", m_tEnv.iBGResourceID);
			Parse_UInt_(tDesc.Params, "ZoneID", m_tEnv.iZoneID);

			m_tEnv.bCanRun = Read_Bool_(tDesc.Params, "CanRun", false);
			m_tEnv.bCanCapture = Read_Bool_(tDesc.Params, "CanCapture", false);
			m_tEnv.bExpGain = Read_Bool_(tDesc.Params, "ExpGain", true);

			if ((BATTLE_RULE::TRAINER_SINGLE == m_tEnv.eRule ||
				BATTLE_RULE::TRAINER_DOUBLE == m_tEnv.eRule) &&
				0 == m_tEnv.iOpponentTrainerID)
				return E_FAIL;

			return S_OK;
		}

		virtual HRESULT Start(EVENT_CONTEXT& tContext) override
		{
			if (nullptr == tContext.pLevelGamePlay)
				return E_FAIL;

			/* Request_Battle()가 전환용 LOCKED를 다시 설정하므로,
			   이벤트 시퀀스가 잡고 있던 입력 잠금은 먼저 해제한다. */
			if (nullptr != tContext.pGameInstance &&
				true == tContext.bInputLockedByEvent)
			{
				tContext.pGameInstance->Set_InputState(tContext.ePrevInputState);
				tContext.bInputLockedByEvent = false;
			}

			if (false == tContext.pLevelGamePlay->Request_Battle(m_tEnv))
				return E_FAIL;

			return S_OK;
		}

		virtual EVENT_PLAY_STATE Update(EVENT_CONTEXT&, _float) override
		{
			return EVENT_PLAY_STATE::FINISHED;
		}

	private:
		BATTLE_ENV m_tEnv = {};
	};
}

const _char* Game_PKM::Get_ActionKindName(EVENT_ACTION_KIND eKind)
{
	switch (eKind)
	{
	case EVENT_ACTION_KIND::NONE:					return "NONE";
	case EVENT_ACTION_KIND::LOCK_INPUT:				return "LockInput";
	case EVENT_ACTION_KIND::RESTORE_INPUT:			return "RestoreInput";
	case EVENT_ACTION_KIND::WAIT_SECONDS:			return "WaitSeconds";
	case EVENT_ACTION_KIND::WAIT_DIALOGUE:			return "WaitDialogue";
	case EVENT_ACTION_KIND::MESSAGE_KEY:			return "MessageKey";
	case EVENT_ACTION_KIND::MESSAGE_TEXT:			return "MessageText";
	case EVENT_ACTION_KIND::CAMERA_PUSH:			return "CameraPush";
	case EVENT_ACTION_KIND::CAMERA_POP:				return "CameraPop";
	case EVENT_ACTION_KIND::CAMERA_SET_POSE:		return "CameraSetPose";
	case EVENT_ACTION_KIND::CAMERA_MOVE_TO:			return "CameraMoveTo";
	case EVENT_ACTION_KIND::CAMERA_BLEND_TO_ACTOR:	return "CameraBlendToActor";
	case EVENT_ACTION_KIND::CAMERA_FOLLOW_ACTOR:	return "CameraFollowActor";
	case EVENT_ACTION_KIND::ACTOR_FACE:				return "ActorFace";
	case EVENT_ACTION_KIND::ACTOR_FACE_YAW:			return "ActorFaceYaw";
	case EVENT_ACTION_KIND::ACTOR_FACE_CAMERA:		return "ActorFaceCamera";
	case EVENT_ACTION_KIND::ACTOR_MOVE_TO:			return "ActorMoveTo";
	case EVENT_ACTION_KIND::ACTOR_SET_ANIM:			return "ActorSetAnim";
	case EVENT_ACTION_KIND::ACTOR_SET_VISIBLE:		return "ActorSetVisible";
	case EVENT_ACTION_KIND::SPAWN_NPC:				return "SpawnNPC";
	case EVENT_ACTION_KIND::DESPAWN_ACTOR:			return "DespawnActor";
	case EVENT_ACTION_KIND::BIND_ACTOR_BY_SPAWN_ID:	return "BindActorBySpawnID";
	case EVENT_ACTION_KIND::REQUEST_BATTLE:			return "RequestBattle";
	case EVENT_ACTION_KIND::DEBUG_LOG:				return "DebugLog";
	default:										return "Unknown";
	}
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
		
	case EVENT_ACTION_KIND::CAMERA_SET_POSE:
		pAction = new CEventAction_CameraSetPose;
		break;

	case EVENT_ACTION_KIND::CAMERA_MOVE_TO:
		pAction = new CEventAction_CameraMoveTo;
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

	case EVENT_ACTION_KIND::BIND_ACTOR_BY_SPAWN_ID:
		pAction = new CEventAction_BindActorBySpawnID;
		break;

	case EVENT_ACTION_KIND::ACTOR_FACE:
		pAction = new CEventAction_ActorFace;
		break;

	case EVENT_ACTION_KIND::ACTOR_FACE_YAW:
		pAction = new CEventAction_ActorFaceYaw;
		break;

	case EVENT_ACTION_KIND::ACTOR_FACE_CAMERA:
		pAction = new CEventAction_ActorFaceCamera;
		break;

	case EVENT_ACTION_KIND::ACTOR_SET_ANIM:
		pAction = new CEventAction_ActorSetAnim;
		break;

	case EVENT_ACTION_KIND::ACTOR_MOVE_TO:
		pAction = new CEventAction_ActorMoveTo;
		break;

	case EVENT_ACTION_KIND::REQUEST_BATTLE:
		pAction = new CEventAction_RequestBattle;
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