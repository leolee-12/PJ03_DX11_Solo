#pragma once
#include "GameObject.h"
#include "Prototype_Manager.h"

/* ------------------------------------------------------------ */
// CGameInstance : 클라이언트에게 엔진 기능을 위한 메서드 제공
/* ------------------------------------------------------------ */

NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
#pragma region ENGINE
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Begin_Draw();
	HRESULT Draw();
	HRESULT End_Draw();
	void Clear_Resources(_int iLevelIndex);
	void Release_Engine();

	const HWND Get_HWND() const;
	_float Random(_float fMin, _float fMax);
#pragma endregion

#pragma region TIMER_MANAGER
	HRESULT Add_Timer(WNameID strTimerTag);
	float Compute_Timer(WNameID strTimerTag);
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT Change_Level(_int iNewLevelIndex, class CLevel* pNewLevel);
	_int Get_CurrentLevel() const;
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	HRESULT Add_Prototype(_uint iLevelIndex, WNameID strProtoTag, CBase* pPrototype);
	CBase* Clone_Prototype(PROTOTYPE eType, _uint iLevelIndex, WNameID strProtoTag, void* pArg = nullptr);
#pragma endregion

#pragma region OBJECT_MANAGER
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, WNameID strProtoTag, _uint iLayerLevelIndex, WNameID strLayerTag, void* pArg = nullptr);
	HRESULT Add_GameObject_Ex(_uint iLayerLevel, WNameID strLayerTag, CGameObject* pObj);
	const list<class CGameObject*>* Get_ObjectList(_uint iLevel, WNameID strLayerTag);
	vector<CGameObject*> Get_LevelObjects(_uint iLevel) const;
#pragma endregion

#pragma region RENDERER
	void Add_RenderGroup(RENDERID eGroupID, class CGameObject* pGameObject);
#pragma endregion

#pragma region PIPELINE
	const _float4x4* Get_Transform(D3DTS eState) const;
	const _float4x4* Get_Transform_Inverse(D3DTS eState) const;
	const _float4* Get_CamPosition() const;
	void Set_CameraWorld(_fmatrix StateMatrix);
	void Set_Projection(_fmatrix StateMatrix);
#pragma endregion

#pragma region INPUT_DEVICE
	void Set_InputState(INPUT_STATE eState);
	INPUT_STATE Get_InputState() const;

	_bool Key_Down(_ubyte byKeyID);
	_bool Key_Up(_ubyte byKeyID);
	_bool Key_Pressing(_ubyte byKeyID);

	_bool Mouse_Down(DIMB eMouse);
	_bool Mouse_Up(DIMB eMouse);
	_bool Mouse_Pressing(DIMB eMouse);
	_long Mouse_Move(DIMM eMouseState);
#pragma endregion

#pragma region LIGHT_MANAGER
	const LIGHT_DESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
#pragma endregion

#pragma region FONT_MANAGER
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
	HRESULT XM_CALLCOV Draw_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition,
		_fvector vColor, _float fRotation, const _float2& vOrigin, const _float2& vScale)
#pragma endregion

private:
	class CGraphic_Device*		m_pGraphic_Device = { nullptr };
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CPrototype_Manager*	m_pPrototype_Manager = { nullptr };
	class CObject_Manager*		m_pObject_Manager = { nullptr };
	class CRenderer*			m_pRenderer = { nullptr };
	class CPipeLine*			m_pPipeLine = { nullptr };
	class CInput_Device*		m_pInput_Device = { nullptr };
	class CLight_Manager*		m_pLight_Manager = { nullptr };
	class CFont_Manager*		m_pFont_Manager = { nullptr };

private:
	virtual void	Free() override;
};

NS_END