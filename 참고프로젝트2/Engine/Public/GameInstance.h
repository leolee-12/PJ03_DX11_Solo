#pragma once

#include "Component_Manager.h"
#include "PipeLine.h"
#include "ThreadPool.h"
#include "Sound_Manager.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	/* 엔진을 초기화한다. */
	HRESULT Initialize_Engine(HINSTANCE hInst, HWND hWnd, _bool isWindowed, _uint iNumLevels, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Render_Engine(_float fTimeDelta);
	HRESULT Clear_LevelResources(_uint iLevelIndex);

public: /* For.Graphic_Device */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	ID3D11ShaderResourceView* Get_BackBufferShaderResourceView();
	HRESULT Present();

public: /* For.Input_Device */
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouseKeyState);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseMoveState);
	_bool Key_Pressing(_uint _iKey);
	_bool Key_Down(_uint _iKey);
	_bool Key_Up(_uint _iKey);
	_bool Mouse_Pressing(_uint _iButton);
	_bool Mouse_Down(_uint _iButton);
	_bool Mouse_Up(_uint _iButton);

public: /* For.Level_Manager */
	HRESULT Change_Level(class CLevel* pNewLevel);
	_uint Get_CurrentLevel_Index();
	_uint Get_LoadingLevel_Index();
	HRESULT Set_LoadingLevel_Index(_uint);
	class CLevel* Get_Level();

public: /* For.Timer_Manager */
	HRESULT Add_Timer(const _wstring& strTimerTag);
	_float Compute_TimeDelta(const _wstring& strTimerTag);
	void StartSlowMotion(_float fTimeScale);
	void StopSlowMotion();
	_float Get_UnscaledDeltaTime(const _wstring& strTimerTag);
	_float Get_ScaledDeltaTime(const _wstring& strTimerTag);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject_ToLayer(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strLayerTag, void* pArg = nullptr);
	CGameObject* Add_GameObject_ToLayer_AndGet(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strLayerTag, void* pArg = nullptr);

	class CComponent* Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	class list<class CGameObject*> Get_Layer(_uint iLevelIndex, const wstring& strLayerTag);
	class CGameObject* Clone_GameObject(const wstring& strPrototypeTag, void* pArg = nullptr);

	void Destory_Reserve(class CGameObject* gameObject);

	class CGameObject* Find_Prototype(const wstring& strPrototypeTag);

	HRESULT Get_Prototype_Names(vector<string>* pVector);
	HRESULT Add_Object_Layers_Vector(_uint iLevelIndex, vector<pair < string, list<CGameObject*>>>*);
	HRESULT Add_Object_Layers_Vector(_uint iLevelIndex, vector<pair < _wstring, list<CGameObject*>>>*);
	_uint Get_LayerSize(_uint iLevelIndex, const wstring& strLayerTag);
	class CGameObject* Get_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex = 0);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	vector<const _wstring*>* Find_Prototype_Include_Key(_uint iLevelIndex, const _wstring& strIncludeTag);

public: /* For.PipeLine */
	_matrix Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_ShadowTransform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_float4x4 Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_float4x4 Get_ShadowTransform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_float4x4 Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_ShadowTransform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_float4x4 Get_ShadowTransform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_vector Get_CamPosition_Vector() const;
	_float4 Get_CamPosition_Float4() const;
	void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);
	void Set_ShadowTransform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);

public: /* For.ThreadPool */
	HRESULT Initialize_ThreadPool(size_t ThreadCount);
	_uint Get_ThreadNumber();

	// 템플릿 EnqueueTask 함수: CThreadPool의 EnqueueTask를 호출
	template<typename FunctionType>
	future<typename result_of<FunctionType()>::type> EnqueueTask(FunctionType task)
	{
		if (m_pThreadPool)
			return m_pThreadPool->EnqueueTask(task);
		else
			throw runtime_error("ThreadPool is not initialized.");
	}

public:/*For. Frustum*/
	void Get_ParallelVectorsInPlane(_float3& tangent1, _float3& tangent2, _float fov);

public: /* For.Collider_Manager*/
	HRESULT Add_ColliderObject(CCollider_Manager::COLLIDERGROUP eRenderGroup, class CCollider* pRenderObject);
	HRESULT Release_Collider(const CCollider*);
	void	Destroy_Reserve(CCollider_Manager::COLLIDERGROUP eRenderGroup);
	void	Destroy_Reserve(CCollider* pCollider);       // 개별 콜라이더 삭제 예약 함수
	HRESULT Destory_ColliderGroup();

public: /* For.FileManager */
	HRESULT SaveObjects(const wstring& filename, void* pArg);
	void* LoadObjects(const wstring& filename);
	HRESULT Save_All_CameraPoints(const wstring& filename, void* pArg);
	HRESULT Load_All_CameraPoints(const std::wstring& filename, CameraSaveData* pArg);
	HRESULT Save_Effects(wstring& FilePath, void* pArg);
	void* Load_Effects(wstring& FilePath);
	void* Load_All_Effects();

public: /* For.Font_Manager */
	HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT Draw_Font(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vFontColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRadian = 0.f, _float2 vPivotPos = _float2(0.f, 0.f), _float fScale = 1.f);
	HRESULT Draw_FontShadow(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vFontColor, _fvector vShadowFontColor, _float fShadowSize, _float fRadian, _float2 vPivotPos, _float fScale);

public: /* For.Sound*/
	void Register_Sound(const std::wstring& filePath, CSound_Manager::SOUND_KEY_NAME alias, CSound_Manager::SOUND_CATEGORY category, _bool loop = false, _bool isNonOverlapping = true);
	void Register_Sound_Group(CSound_Manager::SOUND_GROUP_KEY groupKey, const std::wstring& filePath, CSound_Manager::SOUND_GROUP_KEY_NAME alias, CSound_Manager::SOUND_CATEGORY category, _bool loop);
	void Play_Sound(CSound_Manager::SOUND_KEY_NAME alias, _bool loop, _float volume);
	void Play_Group_Sound(CSound_Manager::SOUND_GROUP_KEY alias, _bool loop, _float volume);
	void Stop_Group_Sound(CSound_Manager::SOUND_GROUP_KEY groupKey);
	void Set_Group_Volume(CSound_Manager::SOUND_GROUP_KEY groupKey, float volume);
	void Stop_Sound(CSound_Manager::SOUND_KEY_NAME alias);
	void Set_ImguiPlay(_bool isPlay);
	void Set_Category_Volume(CSound_Manager::SOUND_CATEGORY category, float volume);
	void Set_Volume(CSound_Manager::SOUND_KEY_NAME alias, float volume);


private:
	class CGraphic_Device* m_pGraphic_Device = { nullptr };
	class CInput_Device* m_pInput_Device = { nullptr };
	class CLevel_Manager* m_pLevel_Manager = { nullptr };
	class CTimer_Manager* m_pTimer_Manager = { nullptr };
	class CObject_Manager* m_pObject_Manager = { nullptr };
	class CComponent_Manager* m_pComponent_Manager = { nullptr };
	class CCollider_Manager* m_pCollider_Manager = { nullptr };
	class CPipeLine* m_pPipeLine = { nullptr };
	class CThreadPool* m_pThreadPool = { nullptr };
	class CFile_Manager* m_pFile_Manager = { nullptr };
	class CFrustum* m_pFrustum = { nullptr };
	class CFont_Manager* m_pFont_Manager = { nullptr };
	class CSound_Manager* m_pSoundManager = { nullptr };

	mutex m_PrototypeMutex; // 프로토타입 추가를 위한 뮤텍스

public:
	void Release_Engine();

	virtual void Free() override;

};

END