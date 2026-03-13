#include "pch.h"
#include "CBaseTool.h"
#include "CRenderer.h"

CBaseTool::CBaseTool(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CBaseTool::CBaseTool(const CBaseTool& rhs)
	: CGameObject(rhs)
{
}

void CBaseTool::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
}

HRESULT CBaseTool::Render_Tool()
{
	ImGui::Render();
	return S_OK;
}

void CBaseTool::Free()
{
	CGameObject::Free();
}
