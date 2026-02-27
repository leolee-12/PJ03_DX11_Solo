#ifdef _DEBUG
#pragma once
#include "Imgui_MapToolBase.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"

BEGIN(Engine)
class CCommonModelComp;
END

BEGIN(Client)

class CImgui_Tab_Object : public CImgui_MapToolBase
{
protected:
	CImgui_Tab_Object(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Tab_Object() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

private:

	_int		m_iModelType = 3;

	string		m_szModeName;
	string		m_szButtonMode = "Model_Tag";

	_bool		m_bButton = false;

	class CImgui_Tab_Instance* m_pTab_Instance = nullptr;
	class CImgui_GroupObject* m_pGroupObject = nullptr;
	class CImgui_Tab_Map_AnimObject* m_pMap_AnimObject = nullptr;
	shared_ptr<class CGameObject> m_pThirdCamera = nullptr;

public:
	HRESULT		Create_Object(wstring& strObjectTag, _float3 vPosition = { 0,0,0 }, void* pArg = nullptr);

	void		RenderListBox();

	void		Delete_Object();

	void		FileSaveLoad();

	void		Shake_Camera();

private:
	string m_strName;
	_float m_fOrigin_Amp = { 0.f };
	_float m_fAmp = { 0.f };
	_float m_fDuration = { 0.f };
	_float m_fSpeed = { 0.f };

	//LightObject
	_int  m_iLightObjectType = { 0 }; // NONE_LIGHTOBJ, WHITE_LIGHTOBJ, YELLOW_LIGHTOBJ, RED_LIGHTOBJ, LIGHTOBJ_TYPEEND

	vector<shared_ptr<CGameObject>>* m_GameObjectList;

public:
	static CImgui_Tab_Object* Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;

};

END

#endif // DEBUG
