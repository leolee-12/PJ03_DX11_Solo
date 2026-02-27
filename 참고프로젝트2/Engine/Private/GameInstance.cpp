#include "..\Public\GameInstance.h"

#include "Component_Manager.h"
#include "Graphic_Device.h"
#include "Object_Manager.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "Input_Device.h"
#include "Collider_Manager.h"
#include "File_Manager.h"
#include "ThreadPool.h"
#include "Frustum.h"
#include "Font_Manager.h"
#include "Sound_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, HWND hWnd, _bool isWindowed, _uint iNumLevels, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_pGraphic_Device = CGraphic_Device::Create(hWnd, isWindowed, iWinSizeX, iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pInput_Device = CInput_Device::Create(hInst, hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pCollider_Manager = CCollider_Manager::Create();
	if (nullptr == m_pCollider_Manager)
		return E_FAIL;

	m_pThreadPool = CThreadPool::Create();
	if (nullptr == m_pThreadPool)
		return E_FAIL;

	m_pFile_Manager = CFile_Manager::Create();
	if (nullptr == m_pFile_Manager)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create();
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pSoundManager = CSound_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pSoundManager)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	m_pInput_Device->Update();

	//전 프레임에서 삭제 예약한 오브젝트 삭제
	m_pObject_Manager->Destory_Update();

	m_pObject_Manager->Player_Update(fTimeDelta);

	m_pSoundManager->Update(fTimeDelta);

	m_pCollider_Manager->Destory_ColliderGroup();

	m_pObject_Manager->Update(fTimeDelta);

	m_pCollider_Manager->Update(fTimeDelta);

	m_pObject_Manager->Late_Update(fTimeDelta);

	m_pObject_Manager->Camera_Update(fTimeDelta);

	m_pPipeLine->Update();
	m_pFrustum->Update();

	m_pLevel_Manager->Update(fTimeDelta);
}

HRESULT CGameInstance::Render_Engine(_float fTimeDelta)
{
	/* 엔진에서 관리하는 객체들 중, 반복적인 렌더가 필요한 객체들이 있다면. */
	/* 여기에서 렌더를 수행해준다. */

	if (FAILED(m_pLevel_Manager->Render(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameInstance::Clear_LevelResources(_uint iLevelIndex)
{

	if (FAILED(m_pObject_Manager->Clear_Resources(iLevelIndex)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Clear_Resources(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

CGameObject* CGameInstance::Find_Prototype(const wstring& strPrototypeTag)
{
	return m_pObject_Manager->Find_Prototype(strPrototypeTag);
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	return m_pGraphic_Device->Clear_DepthStencil_View();
}

ID3D11ShaderResourceView* CGameInstance::Get_BackBufferShaderResourceView()
{
	return m_pGraphic_Device->Get_BackBufferShaderResourceView();
}

HRESULT CGameInstance::Present()
{
	return m_pGraphic_Device->Present();
}


_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouseKeyState)
{
	return m_pInput_Device->Get_DIMouseState(eMouseKeyState);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseMoveState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseMoveState);
}

_bool CGameInstance::Key_Pressing(_uint _iKey)
{
	return m_pInput_Device->Key_Pressing(_iKey);
}

_bool CGameInstance::Key_Down(_uint _iKey)
{
	return m_pInput_Device->Key_Down(_iKey);
}

_bool CGameInstance::Key_Up(_uint _iKey)
{
	return m_pInput_Device->Key_Up(_iKey);
}

_bool CGameInstance::Mouse_Pressing(_uint _iButton)
{
	return m_pInput_Device->Mouse_Pressing(_iButton);
}

_bool CGameInstance::Mouse_Down(_uint _iButton)
{
	return m_pInput_Device->Mouse_Down(_iButton);
}

_bool CGameInstance::Mouse_Up(_uint _iButton)
{
	return m_pInput_Device->Mouse_Up(_iButton);
}

HRESULT CGameInstance::Change_Level(CLevel* pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Change_Level(pNewLevel);
}

_uint CGameInstance::Get_CurrentLevel_Index()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;


	return m_pLevel_Manager->Get_CurrentLevel_Index();
}

_uint CGameInstance::Get_LoadingLevel_Index()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Get_LoadingLevel_Index();
}

HRESULT CGameInstance::Set_LoadingLevel_Index(_uint _level)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Set_LoadingLevel_Index(_level);
}

CLevel* CGameInstance::Get_Level()
{
	if (nullptr == m_pLevel_Manager)
		return nullptr;

	return m_pLevel_Manager->Get_Level();
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

void CGameInstance::StartSlowMotion(_float fTimeScale)
{
	m_pTimer_Manager->Set_TimeScale(fTimeScale);
}

void CGameInstance::StopSlowMotion()
{
	m_pTimer_Manager->Set_TimeScale(1.0f); // 시간 스케일을 정상으로 복원
}

_float CGameInstance::Get_UnscaledDeltaTime(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Get_UnscaledDeltaTime(strTimerTag);
}

_float CGameInstance::Get_ScaledDeltaTime(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Get_ScaledDeltaTime(strTimerTag);
}
HRESULT CGameInstance::Add_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype)
{
	lock_guard<mutex> lock(m_PrototypeMutex);

	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_GameObject_ToLayer(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strLayerTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject_ToLayer(iLevelIndex, strPrototypeTag, strLayerTag, pArg);
}

CGameObject* CGameInstance::Add_GameObject_ToLayer_AndGet(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strLayerTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Add_GameObject_ToLayer_AndGet(iLevelIndex, strPrototypeTag, strLayerTag, pArg);
}

CComponent* CGameInstance::Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return	m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
}

list<class CGameObject*> CGameInstance::Get_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	return m_pObject_Manager->Get_Layer(iLevelIndex, strLayerTag);
}

CGameObject* CGameInstance::Clone_GameObject(const wstring& strPrototypeTag, void* pArg)
{
	return m_pObject_Manager->Clone_GameObject(strPrototypeTag, pArg);
}

void CGameInstance::Destory_Reserve(CGameObject* gameObject)
{
	m_pObject_Manager->Destory_Reserve(gameObject);
}


HRESULT CGameInstance::Get_Prototype_Names(vector<string>* pVector)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return	m_pObject_Manager->Get_Prototype_Names(pVector);
}

HRESULT CGameInstance::Add_Object_Layers_Vector(_uint iLevelIndex, vector<pair<string, list<CGameObject*>>>* pVector)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return	m_pObject_Manager->Add_Object_Layers_Vector(iLevelIndex, pVector);
}

HRESULT CGameInstance::Add_Object_Layers_Vector(_uint iLevelIndex, vector<pair<_wstring, list<CGameObject*>>>* pVector)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return	m_pObject_Manager->Add_Object_Layers_Vector(iLevelIndex, pVector);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CComponent* pPrototype)
{
	lock_guard<mutex> lock(m_PrototypeMutex);

	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

_uint CGameInstance::Get_LayerSize(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->GetLayerSize(iLevelIndex, strLayerTag);
}

CGameObject* CGameInstance::Get_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_GameObject(iLevelIndex, strLayerTag, iIndex);
}

vector<const _wstring*>* CGameInstance::Find_Prototype_Include_Key(_uint iLevelIndex, const _wstring& strIncludeTag)
{
	return m_pComponent_Manager->Find_Prototype_Include_Key(iLevelIndex, strIncludeTag);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}

_matrix CGameInstance::Get_ShadowTransform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_ShadowTransform_Matrix(eState);
}

_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_float4x4 CGameInstance::Get_ShadowTransform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_ShadowTransform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Matrix(eState);
}

_float4x4 CGameInstance::Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Float4x4(eState);
}

_matrix CGameInstance::Get_ShadowTransform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_ShadowTransform_Inverse_Matrix(eState);
}

_float4x4 CGameInstance::Get_ShadowTransform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return  m_pPipeLine->Get_ShadowTransform_Inverse_Float4x4(eState);
}

_vector CGameInstance::Get_CamPosition_Vector() const
{
	return m_pPipeLine->Get_CamPosition_Vector();
}

_float4 CGameInstance::Get_CamPosition_Float4() const
{
	return m_pPipeLine->Get_CamPosition_Float4();
}

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

void CGameInstance::Set_ShadowTransform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	m_pPipeLine->Set_ShadowTransform(eState, TransformMatrix);
}


HRESULT CGameInstance::Initialize_ThreadPool(size_t ThreadCount)
{
	return m_pThreadPool->Initialize(ThreadCount);
}

_uint CGameInstance::Get_ThreadNumber()
{
	return m_pThreadPool->Get_ThreadNumber();
}

void CGameInstance::Get_ParallelVectorsInPlane(_float3& tangent1, _float3& tangent2, _float fov)
{
	m_pFrustum->Get_ParallelVectorsInPlane(tangent1, tangent2, fov);
}

HRESULT CGameInstance::Add_ColliderObject(CCollider_Manager::COLLIDERGROUP eRenderGroup, CCollider* pRenderObject)
{
	return m_pCollider_Manager->Add_ColliderObject(eRenderGroup, pRenderObject);
}

HRESULT CGameInstance::Release_Collider(const CCollider* Collider)
{
	return m_pCollider_Manager->Release_Collider(Collider);
}

void CGameInstance::Destroy_Reserve(CCollider_Manager::COLLIDERGROUP eRenderGroup)
{
	return m_pCollider_Manager->Destroy_Reserve(eRenderGroup);
}

void CGameInstance::Destroy_Reserve(CCollider* pCollider)
{
	return m_pCollider_Manager->Destroy_Reserve(pCollider);
}

HRESULT CGameInstance::Destory_ColliderGroup()
{
	return m_pCollider_Manager->Destory_ColliderGroup();
}


HRESULT CGameInstance::SaveObjects(const wstring& filename, void* pArg)
{
	if (nullptr == m_pFile_Manager)
		return E_FAIL;

	return m_pFile_Manager->SaveObjects(filename, pArg);
}

void* CGameInstance::LoadObjects(const wstring& filename)
{
	if (nullptr == m_pFile_Manager)
		return nullptr;

	return m_pFile_Manager->LoadObjects(filename);
}

HRESULT CGameInstance::Save_All_CameraPoints(const wstring& filename, void* pArg)
{
	return m_pFile_Manager->Save_All_CameraPoints(filename, pArg);
}

HRESULT CGameInstance::Save_Effects(wstring& FilePath, void* pArg)
{
	if (nullptr == m_pFile_Manager)
		return E_FAIL;

	return m_pFile_Manager->Save_Effects(FilePath, pArg);
}

HRESULT CGameInstance::Load_All_CameraPoints(const std::wstring& filename, CameraSaveData* pArg)
{
	if (nullptr == m_pFile_Manager)
		return E_FAIL;

	return m_pFile_Manager->Load_All_CameraPoints(filename, pArg);
}

HRESULT CGameInstance::Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return m_pFont_Manager->Add_Font(pDevice, pContext, strFontTag, pFontFilePath);
}

HRESULT CGameInstance::Draw_Font(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vFontColor, _float fRadian, _float2 vPivotPos, _float fScale)
{
	return m_pFont_Manager->Draw_Font(strFontTag, pText, vPosition, vFontColor, fRadian, vPivotPos, fScale);
}

HRESULT CGameInstance::Draw_FontShadow(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vFontColor, _fvector vShadowFontColor, _float fShadowSize, _float fRadian, _float2 vPivotPos, _float fScale)
{
	return m_pFont_Manager->Draw_FontShadow(strFontTag, pText, vPosition, vFontColor, vShadowFontColor, fShadowSize, fRadian, vPivotPos, fScale);
}

void* CGameInstance::Load_Effects(wstring& FilePath)
{
	if (nullptr == m_pFile_Manager)
		return nullptr;

	return m_pFile_Manager->Load_Effects(FilePath);
}

void CGameInstance::Register_Sound(const std::wstring& filePath, CSound_Manager::SOUND_KEY_NAME alias, CSound_Manager::SOUND_CATEGORY category, _bool loop, _bool isNonOverlapping)
{
	m_pSoundManager->Register_Sound(filePath, alias, category, loop, isNonOverlapping);
}

void CGameInstance::Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY groupKey, const std::wstring& filePath, CSound_Manager::SOUND_GROUP_KEY_NAME alias, CSound_Manager::SOUND_CATEGORY category, _bool loop)
{
	m_pSoundManager->Register_Sound_Group(groupKey, filePath, alias, category, loop);
}

void CGameInstance::Play_Sound(CSound_Manager::SOUND_KEY_NAME alias, _bool loop, _float volume)
{
	m_pSoundManager->Play_Sound(alias, loop, volume);
}

void CGameInstance::Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY alias, _bool loop, _float volume)
{
	m_pSoundManager->Play_Group_Sound(alias, loop, volume);
}

void CGameInstance::Stop_Group_Sound(CSound_Manager::SOUND_GROUP_KEY alias)
{
	m_pSoundManager->Stop_Group_Sound(alias);
}

void CGameInstance::Set_Group_Volume(CSound_Manager::SOUND_GROUP_KEY alias, float volume)
{
	m_pSoundManager->Set_Group_Volume(alias, volume);
}

void CGameInstance::Stop_Sound(CSound_Manager::SOUND_KEY_NAME alias)
{
	m_pSoundManager->Stop_Sound(alias);
}

void CGameInstance::Set_ImguiPlay(_bool isPlay)
{
	m_pSoundManager->Set_ImguiPlay(isPlay);
}

void CGameInstance::Set_Category_Volume(CSound_Manager::SOUND_CATEGORY category, float volume)
{
	m_pSoundManager->Set_Category_Volume(category, volume);
}

void CGameInstance::Set_Volume(CSound_Manager::SOUND_KEY_NAME alias, float volume)
{
	m_pSoundManager->Set_Volume(alias, volume);
}

void* CGameInstance::Load_All_Effects()
{
	if (nullptr == m_pFile_Manager)
		return nullptr;

	return m_pFile_Manager->Load_All_Effects();
}

void CGameInstance::Release_Engine()
{
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pSoundManager);

	CGameInstance::Get_Instance()->Destroy_Instance();
}

void CGameInstance::Free()
{
	__super::Free();
}
