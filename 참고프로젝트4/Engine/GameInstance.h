#pragma once

#include "Renderer.h"
#include "PipeLine.h"
#include "Component_Manager.h"
#include "ShaderMgr_Enum.h"
#include "Object_Manager.h"
#include "Easing.h"
#include "ObjPool_Manager.h"


/* 클라이언트에서 엔진의 기능을 사용하기위해 반드시 거쳐야하는 객체. */

/* DX11을 통해 필요한 객체를 생성하고 렌더링하는 작업을 수행한다. */
/* 지금 생성한 레벨(씬)을 보관하고 필요에따라 업데이트 수행. */
/* 내가 생성한 객체들을 보관하고 업데이트하고 렌더한다. */
/* 내가 생성한 여러 컴포넌트들의 원형을 보관하고 복제하여 가져온다. */

BEGIN(Engine)

class FModelData;
class FMeshData;
class CMeshGroup;
class FMaterialData;

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.Engine */
	/* 엔진라이브러리를 사용하기위한 준비를 모두 거친다. */
	HRESULT	Initialize_Engine(_uint iNumLevels, _uint iNumColLayers, HINSTANCE hInstance, const GRAPHIC_DESC& GraphicDesc, _Inout_ ComPtr<ID3D11Device>* ppDevice, _Inout_ ComPtr<ID3D11DeviceContext>* ppContext);
	void	Tick_Engine(_cref_time fTimeDelta);
	HRESULT Render_Engine();
	void	Clear(_uint iLevelIndex);

public: /* For.Graphic_Device */		
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();	
	HRESULT Present();
	ComPtr<ID3D11RenderTargetView> Get_BackBufferRTV() const;
	ComPtr<ID3D11DepthStencilView> Get_DSV() const;
	ComPtr<ID3D11ShaderResourceView> Get_DS_SRV() const;
	ComPtr<ID3D11Device> Get_D3D11Device() const;
	ComPtr<ID3D11DeviceContext> Get_D3D11Context() const;


public: /* For.Input_Device */
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);

	bool	Key_Pressing(_ubyte byKeyID);
	bool	Key_Down(_ubyte byKeyID);
	bool	Key_Up(_ubyte byKeyID);

	_bool	Key_PressingEx(_ubyte byKeyID, DIK_EX byModifierID = DIK_MD_NONE);
	_bool	Key_DownEx(_ubyte byKeyID, DIK_EX byModifierID = DIK_MD_NONE);
	_bool	Key_UpEx(_ubyte byKeyID, DIK_EX byModifierID = DIK_MD_NONE);

	bool	Mouse_Pressing(MOUSEKEYSTATE eMouse);
	bool	Mouse_Down(MOUSEKEYSTATE eMouse);
	bool	Mouse_Up(MOUSEKEYSTATE eMouse);

	void	Mouse_Fix();
	_bool	Mouse_RayIntersect(SPACETYPE eSpacetype, _float3 * pOut, _fvector vV1, _fvector vV2, _fvector vV3, _matrix matWorld, _float* pLengthOut = nullptr);
	void	Mouse_ToWorld(_vector & vOrigin, _vector & vDir);
	_bool	 Any_Input_Received();

public: /* For.Key_Manager*/
	void Register_Key(_ubyte key);
	_bool Is_KeyRegistered(_ubyte key);
	KEYSTATE Get_KeyState(_ubyte key);
	_bool    Check_KeyState(_ubyte key, KEYSTATE keyState);
	KEYSTATE Get_MouseState(MOUSEKEYSTATE mouse);
	_bool	 Check_MouseState(MOUSEKEYSTATE mouse, KEYSTATE keyState);

public: /* For.Timer_Manager */
	HRESULT	Add_Timer(const wstring& strTimeTag);
	_float Compute_TimeDelta(const wstring& strTimeTag);

	void Save_TimeDelta(_float NowTime);
	_float Get_TimeDelta();


public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iCurrentLevelIndex, class CLevel* pNewLevel);
	_bool	IsVisitedLevel(_uint iCurrentLevelIndex);
	void	Set_VisitedLevel(_uint iCurrentLevelIndex);
	_uint	Get_CreateLevelIndex();
	void	Set_CreateLevelIndex(_uint eLevelIndex);
	class CLevel* Get_CurrentLevel();
	_uint	Get_CurrentLevelIndex();


public: /* For.Object_Manager */
	HRESULT Add_Prototype(const wstring& strPrototypeTag, shared_ptr<class CGameObject> pPrototype);

	template<class T>
	HRESULT Add_CloneObject(_uint iLevelIndex, const LAYERTYPE & strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr, shared_ptr<T>* ppOut = nullptr);

	HRESULT Add_CloneObject(_uint iLevelIndex, const LAYERTYPE& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);

	HRESULT Add_Object(_uint iLevelIndex, const LAYERTYPE & strLayerTag, shared_ptr<class CGameObject> pObject);
	shared_ptr<class CGameObject> CloneObject(const wstring & strPrototypeTag, void* pArg);
	map<const wstring, shared_ptr<class CGameObject>>* Get_ObjectPrototypeList();
	list<shared_ptr<class CGameObject>>* Get_ObjectList(_uint iLevelIndex, const LAYERTYPE & strLayerTag);
	shared_ptr<class CComponent> Get_Component(_uint iLevelIndex, const LAYERTYPE & strLayerTag, const wstring & strComponentTag, _uint iIndex = 0, const wstring & strPartTag = TEXT(""));
	shared_ptr<class CGameObject> Get_Object(_uint iLevelIndex, const LAYERTYPE & strLayerTag, _uint iIndex = 0);


public:	/*For.Event_Manager*/
	void Add_LevelEvent(_uint iLevelIndex, string sEvent, function<_bool(weak_ptr<CBase>, shared_ptr<CGameObject>)> Func);
	void Erase_LevelEvent(_uint iLevelIndex, string sEvent);
	HRESULT Excute_LevelEvent(_uint iLevelIndex, string sEvent, weak_ptr<CBase> _pCaller, shared_ptr<CGameObject> pOther = nullptr);

	void Add_TickEvent(shared_ptr<CBase> _pSubscriber, function<_bool(_cref_time fTimeDelta)> Func);
	void Erase_TickEvent(shared_ptr<CBase> _pSubscriber);
	void Clear_TickEvents();

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag,  shared_ptr<class CComponent> pPrototype);
	shared_ptr<class CComponent> Clone_Component(_uint iLevelIndex, const wstring & strPrototypeTag, shared_ptr<CGameObject> pOwner = nullptr, void* pArg = nullptr);
	map<const wstring, shared_ptr<class CComponent>>* Get_ComponentPrototypeList(_uint iLevelIndex);

public:  /* For.Collision_Manager */
	void Add_Collider(const _uint & iLayer, CCollider * pCollider);
	void Add_LayerGroup(const _uint & iLeftLayer, const _uint & iRightLayer);
	void Reset_CollisionManager();

public:  /* For.Font_Manager */
	HRESULT Add_Font(const wstring & strFontTag, const wstring & strFontFilePath);
	//폰트Tag, 텍스트, 위치(왼쪽정렬), 컬러, 스케일, 회전 중심, 회전각(Radian) 
	HRESULT Render_Font(const wstring & strFontTag, const wstring & strText, const _float2 & vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fScale = 1.f, _float2 vOrigin = _float2(0.f, 0.f), _float fRotation = 0.f);

public: /* For.Renderer */
	class CRenderer* Get_Renderer();
	HRESULT Add_RenderGroup(CRenderer::RENDERGROUP eGroupID, shared_ptr<class CGameObject> pGameObject);
	HRESULT Add_DebugRender(shared_ptr<class CComponent> pDebugCom);
	void Set_RenderType(RENDERTYPE type);
#ifdef _DEBUG
	void	Add_DebugEvent(FastDelegate0<HRESULT> Event);
	void	Toggle_DebugDraw();
#endif // _DEBUG


public: /* For.Target_Manager */
	HRESULT Add_RenderTarget(const wstring & strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4 & vClearColor, _uint iArraySize = 1);
	HRESULT Add_MRT(const wstring & strMRTTag, const wstring & strTargetTag);
	HRESULT Begin_MRT(const wstring & strMRTTag, _bool bClear = true, ComPtr<ID3D11DepthStencilView> pDSV = nullptr);
	HRESULT End_MRT();
	HRESULT Bind_RenderTarget_ShaderResource(const wstring & strTargetTag, class CShader* pShader, const _char * pConstantName);
	class CRenderTarget* Find_RenderTarget(const wstring& strTargetTag);
	HRESULT Bake_AntiAliasingDepthStencilView(_uint iWidth, _uint iHeight);
	ComPtr<ID3D11DepthStencilView> Get_AntiAliasingDepthStencilView();
	D3D11_VIEWPORT Get_AntiAliasingViewPortDesc();

#ifdef _DEBUG
	HRESULT Ready_RenderTarget_Debug(const wstring & strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug_RTVs(const wstring & strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

public: /* For.Light_Manager */
	HRESULT Add_Light(const LIGHT_DESC & LightDesc, shared_ptr<class CLight>* ppOut = nullptr);
	HRESULT Add_Light(string FilePath, vector<shared_ptr<class CLight>>* LightVector = nullptr);

	HRESULT Render_Lights(shared_ptr<class CShader> pShader, shared_ptr<class CVIBuffer_Rect> pVIBuffer);
	void	Release_Light(shared_ptr<class CLight> pLight);
	void	Reset_Light();
	void	Reset_LightManager();
	shared_ptr<class CLight>	Make_Light_On_Owner(weak_ptr<CGameObject> _pOwner, _float4 vColor = { 1.f, 0.f, 0.f, 1.f }, 
		_float fRange = { 3.f }, _float3 vPosOffset = _float3(0.f, 0.f, 0.f), _bool bUseVolumetric = true, _float4 vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f));
	_float4x4 Get_ShadowLight_ViewMatrix();
	_float4x4 Get_ShadowLight_ProjMatrix();
	weak_ptr<class CLight> Get_DirectionalLight();
	shared_ptr<class CLight> Find_Light(string LightName);
	void	Set_LightCulling(_bool bUseCull);

public: /* For.PipeLine */
	void Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix);
	void Set_Transform(CPipeLine::TRANSFORMSTATE eState, _float4x4 TransformMatrix);
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState);
	_float4x4 Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eState);
	_matrix Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE eState);
	_float4x4 Get_TransformFloat4x4Inverse(CPipeLine::TRANSFORMSTATE eState);
	_float4 Get_CamPosition(); 
	void	Set_CamPosition(_float4 vCamPosition);
	_float4 Get_CamLook();
	void	Set_CamLook(_float4 vCamLook);

public: /* For.Texture_Manager */
	friend class CTextureComponent;
public:
	HRESULT Ready_Texture(const wstring& strTexturePath, const _uint iNumTextures, const _bool bIsPermanent, _bool bUseMainPath = true);
	const map<wstring, shared_ptr<class CTexture_Data>>* Get_TexturesMap() const;
	ID3D11ShaderResourceView* Find_Front_SRV(const wstring& strTextureKey);
	HRESULT					Include_All_Textures(const wstring& strMainPath, _bool bSavePath = false, const _bool bPermanent = true);

private:
	class CTexture_Manager*		Get_TextureManager();


#pragma region 모델 매니저
public:		// 모델 매니저
	HRESULT					Load_Model(const wstring& strFileName);
	HRESULT					Load_DirectorySub_Models(const wstring& strFolderPath, _bool bSavePath = false);
	HRESULT					Rebake_MaterialsAll(const wstring& strFolderPath);

	const FModelData* const Find_ModelData(const wstring& strModelKey);
	const FMeshData* const	Find_MeshData(const wstring& strModelKey, const wstring& strMeshKey, const _uint iRangeIndex);
	const FMeshData* const	Find_MeshData(const wstring& strModelKey, const _uint iIndex);
	const CMeshGroup* const Find_MeshGroup(const wstring& strModelKey);
	class CBoneGroup* Clone_BoneGroup(const wstring& strGroupKey);
	class CBoneGroup* Find_BoneGroup(const wstring& strGroupKey);
	class CBoneAnimGroup* Find_AnimGroup(const wstring& strModelKey);
	const FMaterialData* const Find_MaterialData(const wstring& strModelKey, const _uint iIndex);
	const unordered_map<wstring, FModelData*>* Get_ModelDatas();
#pragma endregion


#pragma region 셰이더 매니저
	friend class CShader;
	friend class CComputeShaderComp;
public:
	HRESULT							Load_Shader(const wstring& strFileName, const EShaderType eType, const _char* pEntryPoint);
	
	const ComPtr<ID3DBlob>			Get_ShaderByte(const EShaderType eType, const wstring& strKey);
	template<EShaderType Type>
	inline ComPtr<ShaderType<Type>> Get_ShaderBuffer(const wstring& strKey);
	HRESULT							Load_Effect(const wstring& strFileName, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, const D3D_SHADER_MACRO* pShaderMacro = nullptr);
	HRESULT							Recompile_EffectsAll();
	ID3DX11Effect*					Find_Effect(const wstring& strEffectFileName) const;
	shared_ptr<class FEffectData>	Find_EffectData(const wstring& strEffectFileName, const D3D_SHADER_MACRO* pShaderMacro = nullptr) const;

private:	// 셰이더 매니저 귀속형 객체 전용
	class CShader_Manager* Get_ShaderManager();
#pragma endregion


#pragma region 사운드 매니저
public:		// 사운드 매니저
	_int	Play_Sound(const wstring& strGroupKey, const wstring& strSoundKey, enum class ESoundGroup eSoundGroup, _float fVolume = 1.f);
	_int	Play_3DSound(const wstring& strGroupKey, const wstring& strSoundKey, enum class ESoundGroup eSoundGroup, _float fVolume, _float3 vPos);
	_int	Play_3DSound(const wstring& strGroupKey, vector<string> strSoundKeys,
		ESoundGroup eSoundGroup, shared_ptr<CGameObject> pOwner, _float3 vOffsetPos = _float3(0.f, 0.f, 0.f)
		, _float fVolume = 1.f, FMOD::Channel** ppOutChannel = nullptr);
	_int	Play_3DSound(vector<string> strGroupKeys, vector<string> strSoundKeys,
		ESoundGroup eSoundGroup, shared_ptr<CGameObject> pOwner, _float3 vOffsetPos = _float3(0.f, 0.f, 0.f)
		, _float fVolume = 1.f, FMOD::Channel** ppOutChannel = nullptr);
	_int	Play_3DSound(vector<string> strGroupKeys, vector<string> strSoundKeys, ESoundGroup eSoundGroup
		, _float3 vPos
		, _float fVolume = 1.f, _float f3DMin = 0.5f, _float f3DMax = 50.f, FMOD_MODE eRollOffMode = FMOD_3D_LINEARSQUAREROLLOFF,
		FMOD::Channel** ppOutChannel = nullptr);

	_bool	IsPlaying_Channel(_int* pChannelIndex, FMOD::Channel** ppChannel);
	void	Sound_Move(_int* pChannelIndex, FMOD::Channel** ppChannel, _float3 vPos);
	void	Sound_StopByChannel(_int* pChannelIndex, FMOD::Channel** ppChannel);

	_int	Play_BGM(const wstring& strGroupKey, const wstring& strSoundKey, _float fVolume = 1.f);
	_int	Play_BGM_LoopRange(const wstring& strGroupKey, const wstring& strSoundKey, _float fVolume, _uint iStart, _uint iEnd);
	void	FadeOut_BGM(_float fFadeOutSpeed = 1.f);
	void	Stop_BGM();
	void	Stop_SoundAll();
	void	Set_ChannelVolume(_int eID, _float fVolume);
	// 채널 그룹 볼륨 세팅 관련
	_float	Get_MasterVolume() const;
	void	Set_MasterVolume(_float fVolume);
	_float	Get_BGMVolume() const;
	void	Set_BGMVolume(_float fVolume);
	_float	Get_VoiceVolume() const;
	void	Set_VoiceVolume(_float fVolume);
	_float	Get_EffectVolume() const;
	void	Set_EffectVolume(_float fVolume);
	void	Set_ListenerAttributeByMatrix(_fmatrix Matrix);
	void	NormalModeDSP();
	void	WaitModeDSP();

	const _unmap<wstring, shared_ptr<class FSoundData>>* CRef_SoundMap() const;
	vector<wstring> Provide_GroupSoundNames(const wstring& strGroupKey) const;
#pragma endregion

#pragma region 사운드 포인트 매니저
public:
	HRESULT Add_SoundPoint(const string& strName, const struct TSoundPointDesc& Desc, shared_ptr<class CSoundPoint>* ppOut = nullptr);
	HRESULT Add_SoundPoint(const wstring& FilePath, vector<shared_ptr<class CSoundPoint>>* SoundPointVector = nullptr);

	_bool Exists_SoundPointName(const string& strName);

	HRESULT Render_SoundPoints();
	void	Release_SoundPoint(shared_ptr<class CSoundPoint> pSoundPoint);
	void	Reset_SoundPoints();
	void	Reset_SoundPointManager();

	shared_ptr<class CSoundPoint> Find_SoundPoint(const string& Name);

	const _bool Get_Dead();

#pragma endregion



#pragma region 쓰레드 풀
public:
	class CThreadPool* Get_ThreadPool();
#pragma endregion

#pragma region 절두체
public:
	void	Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrix);
	const BoundingFrustum* Get_WorldFrustumPtr();
#pragma endregion



public:/* ETC */
#ifdef _DEBUG
	void Render_FPS(_cref_time fTimeDelta, HWND hwnd, wstring strLevelName = TEXT(""));
#endif
	template<typename T>
	typename std::enable_if<std::is_arithmetic<T>::value, T>::type
		Random(T low, T high);
	template<typename T>
	typename std::enable_if<std::is_arithmetic<T>::value, T>::type
		RandomInt(T low, T high);
	
	XMFLOAT3 RandomFloat3(XMFLOAT3 low, XMFLOAT3 high);
	XMFLOAT4 RandomFloat4(XMFLOAT4 low, XMFLOAT4 high);
	void Random_Vector(vector<_float>& Out_values, _int num, _float low, _float high);
	void RandomInt_Vector(vector<_int>& Out_values,_int num, _int low, _int high);
	void RandomFloat3_Vector(vector<_float3>& Out_values, _int num, _float3 low, _float3 high);
	string RandomString(vector<string> Strings);


	// 비동기 처리용 셰어드 뮤텍스, 비동기 처리가 필요하면 언제든 이걸 가져다 쓴다.
	shared_mutex& Get_AsncMutex() { return m_AsyncMutex; }
	const wstring& MainBinPath() const { return m_strMainBinPath; }
	const wstring& MainDataPath() const { return m_strMainDataPath; }
	const wstring& MainResourcePath() const { return m_strMainResourcePath; }
	const wstring& MainJSONScriptPath() const { return m_strMainJSONScriptPath; }

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CEvent_Manager*			m_pEvent_Manager = { nullptr };
	class CCollision_Manager*		m_pCollision_Manager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CTexture_Manager*			m_pTexture_Manager = { nullptr };
	class CModel_Manager*			m_pModel_Manager = {nullptr};
	class CShader_Manager*			m_pShader_Manager = { nullptr };
	class CSound_Manager*			m_pSound_Manager = { nullptr };
	class CKey_Manager*				m_pKey_Manager = { nullptr };
	class CThreadPool*				m_pThreadPool = { nullptr };
	class CSoundPoint_Manager*		m_pSoundPoint_Manager = { nullptr };

	class CFrustum*					m_pFrustum = { nullptr };

	random_device					m_RandomDevice;
	mt19937_64						m_RandomNumber;
	
	_float							m_fNowTimeDelta_60;
	shared_mutex					m_AsyncMutex;			// 비동기 처리를 위한 뮤텍스, shared_lock으로 읽기, unique_lock으로 쓰기 락을 획득하여 사용한다.
	wstring							m_strMainBinPath = TEXT("");
	wstring							m_strMainDataPath = TEXT("");
	wstring							m_strMainResourcePath = TEXT("");
	wstring							m_strMainJSONScriptPath = TEXT("");

	weak_ptr<class CCamera>			m_pCurCamera;

public:
	GETSET_EX2(weak_ptr<class CCamera>, m_pCurCamera, CurCamera, GET_C_REF, SET)

public:
	static _float m_fAdjustTimeDelta;
	static _bool m_bPauseGame;
	static _bool m_bIsFocusedWindow;

public:
	void Release_Manager();
	static void Release_Engine();
};


template<class T>
HRESULT CGameInstance::Add_CloneObject(_uint iLevelIndex, const LAYERTYPE& strLayerTag, const wstring& strPrototypeTag, void* pArg, shared_ptr<T>* ppOut)
{
	if (nullptr == m_pObject_Manager)
		RETURN_EFAIL;

	return m_pObject_Manager->Add_CloneObject(iLevelIndex, strLayerTag, strPrototypeTag, pArg, ppOut);
}

// 유틸리티
inline CGameInstance* GI()
{
	return CGameInstance::GetInstance();
}

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type
CGameInstance::Random(T low, T high)
{
	uniform_real_distribution<T> RandomRange(low, high);
	return RandomRange(m_RandomNumber);
}

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type
CGameInstance::RandomInt(T low, T high)
{
	uniform_int_distribution<T> RandomRange(low, high);
	return RandomRange(m_RandomNumber);
}

// 셰이더 종류별로 로드
template<EShaderType Type>
inline ComPtr<ShaderType<Type>> CGameInstance::Get_ShaderBuffer(const wstring& strKey)
{
	if (nullptr == m_pShaderMgr)
		return nullptr;

	return m_pShaderMgr->Get_ShaderBuffer<Type>(strKey);
}



END

