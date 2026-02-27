#pragma once

#ifdef _DEBUG
#include "Imgui_Tab.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"

BEGIN(Client)

/// <summary>
/// 오프셋 표시용
/// </summary>
class CImgui_Tab_AnimOffset : public CImgui_Tab
{
	INFO_CLASS(CImgui_Tab_AnimOffset, CImgui_Tab)

private:
	CImgui_Tab_AnimOffset(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Tab_AnimOffset() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

public:
	static CImgui_Tab_AnimOffset* Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;

public:
	const _float4x4& Get_OffsetMatrix() const { return m_OffsetMatrix; }

private:
	_float4x4		m_OffsetMatrix = {};

};

END
#endif