#include "EventSequence_Player.h"
#include "Event_Definition.h"
#include "EventAction.h"
#include "Camera_Free.h"

#include "GameInstance.h"
#include "Transform.h"

namespace
{
    void Restore_CameraSnapshot_(EVENT_CONTEXT& tContext)
    {
        if (false == tContext.tCameraSnapshot.bValid)
            return;

        if (nullptr == tContext.pGameInstance)
            return;

        CCamera* pCamera = tContext.pGameInstance->Get_MainCamera();
        if (nullptr == pCamera || nullptr == pCamera->Get_Transform())
            return;

        CTransform* pTransform = pCamera->Get_Transform();

        pTransform->Set_State(
            STATE::POSITION,
            XMVectorSetW(XMLoadFloat3(&tContext.tCameraSnapshot.vEye), 1.f));

        pTransform->LookAt(
            XMVectorSetW(XMLoadFloat3(&tContext.tCameraSnapshot.vAt), 1.f));

        pCamera->Set_Following(tContext.tCameraSnapshot.bFollowing);
        pCamera->Set_ControlEnabled(tContext.tCameraSnapshot.bControlEnabled);

        if (CCamera_Free* pFreeCamera = dynamic_cast<CCamera_Free*>(pCamera))
            pFreeCamera->Flush_PipeLine();
        else
            pCamera->Update(0.f);

        tContext.tCameraSnapshot = {};
    }
}

CEventSequence_Player::CEventSequence_Player()
{
}

HRESULT CEventSequence_Player::Initialize(const CEvent_Definition* pSequence, const EVENT_CONTEXT&
    tContext)
{
    if (nullptr == pSequence)
        return E_FAIL;

    m_pSequence = pSequence;
    m_tContext = tContext;

    m_iGroupIndex = 0;
    m_iStepIndex = 0;
    m_bCurrentStarted = false;
    m_eState = EVENT_PLAY_STATE::PLAYING;

    return S_OK;
}

EVENT_PLAY_STATE CEventSequence_Player::Update(_float fTimeDelta)
{
    if (EVENT_PLAY_STATE::PLAYING != m_eState &&
        EVENT_PLAY_STATE::WAITING != m_eState)
        return m_eState;

    if (nullptr == m_pSequence)
    {
        m_eState = EVENT_PLAY_STATE::FAILED;
        return m_eState;
    }

    const vector<EVENT_STEP_GROUP>& Groups = m_pSequence->Get_Groups();
    if (m_iGroupIndex >= Groups.size())
    {
        m_eState = EVENT_PLAY_STATE::FINISHED;
        return m_eState;
    }

    const EVENT_STEP_GROUP& tGroup = Groups[m_iGroupIndex];

    /* E1에서는 SEQUENTIAL만 처리한다. PARALLEL은 E6에서 확장. */
    if (EVENT_STEP_MODE::SEQUENTIAL != tGroup.eMode)
    {
        m_eState = EVENT_PLAY_STATE::FAILED;
        return m_eState;
    }

    if (m_iStepIndex >= tGroup.Steps.size())
    {
        ++m_iGroupIndex;
        m_iStepIndex = 0;
        Release_CurrentAction();
        return Update(fTimeDelta);
    }

    if (nullptr == m_pCurrentAction)
    {
        const EVENT_STEP_DESC& tStep = tGroup.Steps[m_iStepIndex];

#ifdef _DEBUG
        {
            _string strLog = "[Event] Step Begin group=" + to_string(m_iGroupIndex) +
                " step=" + to_string(m_iStepIndex) +
                " kind=" + Get_ActionKindName(tStep.eKind) +
                " params=" + to_string(tStep.Params.size()) + "\n";
            OutputDebugStringA(strLog.c_str());
        }
#endif

        m_pCurrentAction = CEventAction::Create_Action(tStep);

        if (nullptr == m_pCurrentAction)
        {
#ifdef _DEBUG
            _string strLog = "[Event Warn] Create_Action returned nullptr (kind=" +
                _string(Get_ActionKindName(tStep.eKind)) + ")\n";
            OutputDebugStringA(strLog.c_str());
#endif
            m_eState = EVENT_PLAY_STATE::FAILED;
            return m_eState;
        }

        m_bCurrentStarted = false;
    }

    if (false == m_bCurrentStarted)
    {
        if (FAILED(m_pCurrentAction->Start(m_tContext)))
        {
#ifdef _DEBUG
            _string strLog = "[Event Warn] Action Start FAILED (kind=" +
                _string(Get_ActionKindName(m_pCurrentAction->Get_Kind())) + ")\n";
            OutputDebugStringA(strLog.c_str());
#endif
            m_eState = EVENT_PLAY_STATE::FAILED;
            return m_eState;
        }

        m_bCurrentStarted = true;
    }

    const EVENT_PLAY_STATE eActionState = m_pCurrentAction->Update(m_tContext, fTimeDelta);

    if (EVENT_PLAY_STATE::FAILED == eActionState ||
        EVENT_PLAY_STATE::CANCELED == eActionState)
    {
#ifdef _DEBUG
        _string strLog = (EVENT_PLAY_STATE::FAILED == eActionState
            ? "[Event Warn] Action Update FAILED (kind="
            : "[Event] Action Update CANCELED (kind=") +
            _string(Get_ActionKindName(m_pCurrentAction->Get_Kind())) + ")\n";
        OutputDebugStringA(strLog.c_str());
#endif
        m_eState = eActionState;
        return m_eState;
    }

    if (EVENT_PLAY_STATE::FINISHED == eActionState)
    {
#ifdef _DEBUG
        _string strLog = "[Event] Step Finish group=" + to_string(m_iGroupIndex) +
            " step=" + to_string(m_iStepIndex) +
            " kind=" + Get_ActionKindName(m_pCurrentAction->Get_Kind()) + "\n";
        OutputDebugStringA(strLog.c_str());
#endif
        Release_CurrentAction();
        ++m_iStepIndex;
        m_eState = EVENT_PLAY_STATE::PLAYING;
        return m_eState;
    }

    m_eState = EVENT_PLAY_STATE::WAITING;
    return m_eState;
}

void CEventSequence_Player::Cancel(_bool bSkipGameInstanceCalls)
{
    if (true == bSkipGameInstanceCalls)
        m_bShuttingDown = true;

    if (nullptr != m_pCurrentAction)
        m_pCurrentAction->Cancel(m_tContext);

    Release_CurrentAction();

    if (false == m_bShuttingDown)
    {
        Restore_CameraSnapshot_(m_tContext);

        if (nullptr != m_tContext.pGameInstance && true == m_tContext.bInputLockedByEvent)
        {
            m_tContext.pGameInstance->Set_InputState(m_tContext.ePrevInputState);
            m_tContext.bInputLockedByEvent = false;
        }
    }

    m_eState = EVENT_PLAY_STATE::CANCELED;
}

void CEventSequence_Player::Release_CurrentAction()
{
    Safe_Release(m_pCurrentAction);
    m_bCurrentStarted = false;
}

CEventSequence_Player* CEventSequence_Player::Create(const CEvent_Definition* pSequence, const
    EVENT_CONTEXT& tContext)
{
    CEventSequence_Player* pInstance = new CEventSequence_Player();

    if (FAILED(pInstance->Initialize(pSequence, tContext)))
    {
        MSG_BOX("Failed to Created : CEventSequence_Player");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEventSequence_Player::Free()
{
    Cancel(true);   // 셧다운 모드: GameInstance/Input_Device 호출 skip
    m_pSequence = nullptr;

    __super::Free();
}