#include "Level_Capture.h"
#include "Capture_Manager.h"
#include "Camera_Free.h"

#include "GameInstance.h"

NS_BEGIN(Game_PKM)
static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::CAPTURE);
NS_END

CLevel_Capture::CLevel_Capture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CAPTURE_ENV& tEnv)
	: CLevel{ pDevice, pContext }
	, m_tEnv{ tEnv }
{
}

HRESULT CLevel_Capture::Initialize()
{
	wchar_t szLog[160] = {};
	swprintf_s(szLog,
		L"[Level_Capture] Initialize: SpeciesID=%u, Level=%u, BallItemID=%u, ZoneID=%u\n",
		m_tEnv.iSpeciesID, m_tEnv.iLevel, m_tEnv.iInitialBallItemID, m_tEnv.iZoneID);
	OutputDebugStringW(szLog);

	m_pCaptureManager = CCapture_Manager::Create(m_tEnv);
	if (nullptr == m_pCaptureManager)
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_CAMERA)))
		return E_FAIL;

	m_pCaptureManager->Begin();

	return S_OK;
}

void CLevel_Capture::Update(_float fTimeDelta)
{
	/* 강제 종료 디버그 경로 — 본격 입력 동선이 정리되면 제거. */
	if (m_pGameInstance->Key_Down(DIK_ESCAPE))
	{
		OutputDebugStringW(L"[Level_Capture] ESC pressed → Pop_Level\n");

		if (FAILED(m_pGameInstance->Pop_Level()))
		{
			MSG_BOX("Failed to Exit Capture");
			return;
		}
		return;
	}

	if (nullptr == m_pCaptureManager)
		return;

	m_pCaptureManager->Update(fTimeDelta);

	if (m_pCaptureManager->Is_Done())
	{
		/* Pop_Level 성공 시 본 레벨(=this) 이 즉시 Free 되므로
		   호출 후 어떤 멤버에도 접근하지 않고 곧바로 return. */
		if (FAILED(m_pGameInstance->Pop_Level()))
		{
			MSG_BOX("Failed to Exit Capture");
			return;
		}
		return;
	}
}

HRESULT CLevel_Capture::Render()
{
#ifdef _DEBUG
	if (nullptr != m_pCaptureManager)
	{
		_wstring strTitle = TEXT("Capture Level | Phase: ");
		strTitle += to_wstring(static_cast<_uint>(m_pCaptureManager->Get_Phase()));
		strTitle += TEXT(" | Result: ");
		strTitle += to_wstring(static_cast<_uint>(m_pCaptureManager->Get_Result()));
		SetWindowText(m_pGameInstance->Get_HWND(), strTitle.c_str());
	}
	else
	{
		SetWindowText(m_pGameInstance->Get_HWND(), TEXT("Capture Level (manager null)"));
	}
#endif

	return S_OK;
}

HRESULT CLevel_Capture::Ready_Layer_Camera(WNameID strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC CameraDesc = {};

	CameraDesc.vEye = _float3(-1.3f, 3.2f, -7.3f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(35.f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(180.f);
	CameraDesc.fMouseSensor = 0.03f;

	if (FAILED(m_pGameInstance->Add_GameObject(
		ETOUI(LEVEL::STATIC), PROTO_OBJ_CAMERA_FREE,
		CURRENT_LEVEL, strLayerTag,
		&CameraDesc)))
		return E_FAIL;

	return S_OK;
}

CLevel_Capture* CLevel_Capture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LEVEL_ENTRY_DESC* pEntryDesc)
{
	if (nullptr == pEntryDesc)
	{
		MSG_BOX("Capture entry desc is missing");
		return nullptr;
	}

	const void* pPayload = pEntryDesc->Get_Payload(LEVEL_ENTRY_PAYLOAD::CAPTURE_ENV, sizeof(CAPTURE_ENV));
	if (nullptr == pPayload)
	{
		MSG_BOX("Capture env payload is invalid");
		return nullptr;
	}

	const CAPTURE_ENV& tEnv = *static_cast<const CAPTURE_ENV*>(pPayload);

	CLevel_Capture* pInstance = new CLevel_Capture(pDevice, pContext, tEnv);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Capture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Capture::Free()
{
	Safe_Release(m_pCaptureManager);

	__super::Free();
}