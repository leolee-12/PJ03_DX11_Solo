#include "Level_Capture.h"
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

	if (FAILED(Ready_Layer_Camera(LAYER_CAMERA)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Capture::Update(_float fTimeDelta)
{
	/* 프레임 단위 임시 Pop. 단위 C(CCapture_Manager) 도입 후
	   Is_Done() 체크로 교체. */
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
}

HRESULT CLevel_Capture::Render()
{
#ifdef _DEBUG
	SetWindowText(m_pGameInstance->Get_HWND(), TEXT("Capture Level (frame stub)"));
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
	__super::Free();
}