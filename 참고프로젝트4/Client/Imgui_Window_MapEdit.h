#ifdef _DEBUG
#pragma once
#include "Imgui_Window.h"

BEGIN(Client)

class CImgui_Window_MapEdit : public CImgui_Window
{
private:
	CImgui_Window_MapEdit(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Window_MapEdit() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

public:
	//class CImgui_Tab_Terrain* m_pTerrainTab = { nullptr };
	class CImgui_Tab_Light* m_pLightTab = { nullptr };
	class CImgui_Tab_Trigger* m_pTriggerTab = { nullptr };
	class CImgui_Tab_Object* m_pObjectTab = { nullptr };
	class CImgui_Tab_Collider* m_pColliderTab = { nullptr };
	class CImgui_Tab_Sound* m_pSoundTab = { nullptr };

public:
	_bool	m_bTerrainTabOn = true;
	_bool	m_bObjectTabOn = false;
public:
	static CImgui_Window_MapEdit* Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;
};

END

#endif // DEBUG
