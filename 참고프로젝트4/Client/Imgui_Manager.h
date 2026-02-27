#ifdef _DEBUG
#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
END

BEGIN(Client)

class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)

public:
	enum TOOL { TOOL_MAP, TOOL_EFFECT, TOOL_ANIM, TOOL_UI, TOOL_SHADER, TOOL_CAMERA, TOOL_END };

private:
	CImgui_Manager();
	virtual	~CImgui_Manager() = default;

public:
	HRESULT		Initialize(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	void		Tick(_cref_time fTimeDelta);
	HRESULT		Render();
	void		Setup_ImguiStyle();

public:
	template<typename T>
	T* Find_Window(TOOL eToolWindow);

private:
	ComPtr<ID3D11Device>				m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>			m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

	vector<class CImgui_Window*>		m_pWindows[TOOL_END];
	TOOL								m_eCurrentTool = TOOL_MAP;

	vector<shared_ptr<CGameObject>>		m_GameObjectList[LAYERTYPE_END];

public:
	_bool		Picking_Object(SPACETYPE eSpacetype, LAYERTYPE eLayerType, _float3* pMousePos, _int* pPickIndex = nullptr, _float* pLength = nullptr, shared_ptr<CGameObject>* pPickTerrain = nullptr);
	void		Update_ViewProj();
	void		DrawGrid();
	_bool		Find_ObjectTag(LAYERTYPE eLayerType, wstring strTag, shared_ptr<CGameObject> pGameObject = nullptr);
	void		Dialog_Active();
	void		Update_List(LAYERTYPE eLayerType);

public:
	void		Arrow_Button(const string& strTag, _float fInterval, float* fValue);
	void		Arrow_Button(const string& strTag, _int iInterval, _int* iValue);

public:
	_bool		InputFloat(const string& strTag, _float* vValue);
	_bool		InputFloat2(const string& strTag, _float2* vValue);
	_bool		InputFloat3(const string& strTag, _float3* vValue);
	_bool		InputFloat4(const string& strTag, _float4* vValue);

public:
	void		Load_Image(const wstring& strTextureKey, _float2 vSize);
	// 텍스쳐 이미지를 IMGUI를 통해서 출력 -> 보조 이미지

	// 마우스 영역 제한 시 활용
	_bool		Check_ImGui_Rect();  //마우스 포인터가 ImGui 창 위에 있는지
	_bool		Check_Window_Rect(); //마우스 포인터가 Window(클라 창) 내에 있는지


	void		Load_EaseImage(_uint iEaseIndex, _float2(vSize));

	string		Cut_DecimalPt(string num, int pos)
	{
		return num.substr(0, num.find('.') + pos + 1);
	}


public:
	_float* m_arrView = { nullptr };
	_float* m_arrProj = { nullptr };

#pragma region ImGuizmo Variable

	const float identityMatrix[16] =
	{
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};

#pragma endregion

public:
	virtual	void	Free() override;

};

END

template<typename T>
inline T* CImgui_Manager::Find_Window(TOOL eToolWindow)
{
	auto& vecWindows = m_pWindows[ECast(eToolWindow)];
	for (size_t i = 0; i < vecWindows.size(); i++)
	{
		T* pWindow = DynCast<T*>(vecWindows[i]);
		if (nullptr != pWindow)
			return pWindow;
	}

	return nullptr;
}

#endif // DEBUG
