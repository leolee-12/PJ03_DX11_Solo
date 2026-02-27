#pragma once
#ifdef _DEBUG

#include "Imgui_Window.h"

BEGIN(Client)

class CImgui_Window_AnimEdit : public CImgui_Window
{
	INFO_CLASS(CImgui_Window_AnimEdit, CImgui_Window)

private:
	CImgui_Window_AnimEdit(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Window_AnimEdit() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

public:
	class CImgui_Tab_AnimObject* Tab_AnimObject() { return m_pTab_AnimObject; }
	class CImgui_Tab_AnimOffset* Tab_AnimOffset() { return m_pTab_AnimOffset; }
	class CImgui_Tab_AnimAdd* Tab_AnimAdd() { return m_pTab_AnimAdd; }
	class CImgui_Tab_AnimMask* Tab_AnimMask() { return m_pTab_AnimMask; }

private:
	class CImgui_Tab_AnimObject*	m_pTab_AnimObject = { nullptr };
	class CImgui_Tab_PartsObject*	m_pTab_PartsObject = { nullptr };
	class CImgui_Tab_AnimOffset*	m_pTab_AnimOffset = { nullptr };
	class CImgui_Tab_AnimAdd*		m_pTab_AnimAdd = { nullptr };
	class CImgui_Tab_AnimMask*		m_pTab_AnimMask = { nullptr };

public:
	_bool	m_bSequencer = true;
	_bool	m_bOffsetTab = false;

public:
	static CImgui_Window_AnimEdit* Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;



};

END
#endif // DEBUG
