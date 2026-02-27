#include "stdafx.h"

#include "UIObject.h"
#include "RenderInstance.h"
#include "Character.h"

CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{ pDevice , pContext }
	, m_pUI_Manager{ CUI_Manager::Get_Instance() }
{
	Safe_AddRef(m_pUI_Manager);
}

CUIObject::CUIObject(const CUIObject& Prototype)
	:CGameObject{ Prototype }
	, m_pUI_Manager{ CUI_Manager::Get_Instance() }
{
	Safe_AddRef(m_pUI_Manager);
}

HRESULT CUIObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	
	pDesc = static_cast<UI_DESC*>(pArg);

	m_vPrevWinSize = { 1280.f, 720.f };

	m_vOffSetWinSize.x = g_iWinSizeX / m_vPrevWinSize.x;
	m_vOffSetWinSize.y = g_iWinSizeY / m_vPrevWinSize.y;

	if (pDesc != nullptr)
	{
		m_eLRPos = pDesc->eLRPos;
	
		switch (m_eLRPos)
		{
		case LEFT:
			m_pMainPawn = m_pUI_Manager->m_pPawnArray[CUI_Define::LPLAYER1];
			m_pSubPawn = m_pUI_Manager->m_pPawnArray[CUI_Define::LPLAYER2];
			break;
	
		case RIGHT:
			m_fPosX = m_vPrevWinSize.x - m_fPosX;
			m_fSizeX *= -1;
			m_pMainPawn = m_pUI_Manager->m_pPawnArray[CUI_Define::RPLAYER1];
			m_pSubPawn = m_pUI_Manager->m_pPawnArray[CUI_Define::RPLAYER2];
			break;
		}
	}

	m_bIsActive = true;
	m_pUI_Manager->m_bActive = m_bIsActive;
	return S_OK;
}

void CUIObject::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUIObject::Update(_float fTimeDelta)
{
	InitPlayer();
}

void CUIObject::Late_Update(_float fTimeDelta)
{
	if (m_pMainPawn != nullptr)
		m_bCharaStun = m_pMainPawn->Get_PawnDesc().bStun;
}

HRESULT CUIObject::Render(_float fTimeDelta)
{
	return S_OK;
}

void CUIObject::Set_UI_Setting(_float fSizeX, _float fSizeY, _float fPosX, _float fPosY,  _float fDepth)
{
	fSizeX *= m_vOffSetWinSize.x;
	fSizeY *= m_vOffSetWinSize.y;
	fPosX *= m_vOffSetWinSize.x;
	fPosY *= m_vOffSetWinSize.y;

	if(InManager == FALSE)
		UI_PosArea(fPosY);

	m_pTransformCom->Set_Scaled(fSizeX, fSizeY, 1.f);
	_float fOffSet = -3.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosX - g_iWinSizeX * 0.5f, -fPosY + g_iWinSizeY * 0.5f, fDepth, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
}

void CUIObject::DebugTesting(_float fSizeOffset, _float fDepth)
{
	m_pUI_Manager->UI_Setting_Debug(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, fSizeOffset);
	Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY , fDepth);
}

_bool CUIObject::ClickRange(_float fPickPosX, _float fPickPosY)
{	
	_float MouseX = fPickPosX;
	_float MouseY = fPickPosY;

	_float fPosX = m_fPosX * m_vOffSetWinSize.x;
	_float fPosY = m_fPosY * m_vOffSetWinSize.y;

	_float fSizeX = m_fSizeX * m_vOffSetWinSize.x;
	_float fSizeY = m_fSizeY * m_vOffSetWinSize.y;

	_bool bLeft = (fPosX - fSizeX * 0.5f < MouseX);
	_bool bRight = (fPosX + fSizeX * 0.5f > MouseX);
	_bool bTop = (fPosY - fSizeY * 0.5f < MouseY);
	_bool bBot = (fPosY + fSizeY * 0.5f > MouseY);

	if (bLeft && bRight && bTop && bBot)
		return true;
	

	return false;
}

void CUIObject::InitPlayer()
{
	if (pDesc != nullptr)
	{
		switch (m_eLRPos)
		{
		case LEFT:
			m_pMainPawn = m_pUI_Manager->m_pPawnArray[CUI_Define::LPLAYER1];
			m_pSubPawn = m_pUI_Manager->m_pPawnArray[CUI_Define::LPLAYER2];
			break;

		case RIGHT:
			m_pMainPawn = m_pUI_Manager->m_pPawnArray[CUI_Define::RPLAYER1];
			m_pSubPawn = m_pUI_Manager->m_pPawnArray[CUI_Define::RPLAYER2];
			break;
		}
	}
}

void CUIObject::Add_UIList(CUI_Define::UI_LISTPOS ePos)
{
	switch (ePos)
	{
	case CUI_Define::TOP:
		m_pUI_Manager->m_ListTopUI.push_back(this);
		break;

	case CUI_Define::MID:
		m_pUI_Manager->m_ListMidUI.push_back(this);
		break;

	case CUI_Define::BOT:
		m_pUI_Manager->m_ListBotUI.push_back(this);
		break;

	}
}

void CUIObject::UI_PosArea(_float fAreaPosY)
{
	
	if (fAreaPosY >= 450.f * m_vOffSetWinSize.y)
	{
		Add_UIList(CUI_Define::BOT);
	}
	else if (fAreaPosY >= 160.f * m_vOffSetWinSize.y)
	{
		Add_UIList(CUI_Define::MID);
	}
	else
	{
		Add_UIList(CUI_Define::TOP);
	}
	InManager = TRUE;
}

_bool CUIObject::MoveAnimUI(_vector vTargetPos, _float fSpeed, _float fDepth ,  _float fTimeDelta, _float fEndDistance)
{
	vTargetPos = XMVectorSetX(vTargetPos, XMVectorGetX(vTargetPos) - g_iWinSizeX * 0.5f);
	vTargetPos = XMVectorSetY(vTargetPos, -XMVectorGetY(vTargetPos) + g_iWinSizeY * 0.5f);

	_vector vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vMoveDir = XMVector2Normalize(vTargetPos - vOriginPos);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vMoveDir * fSpeed * fTimeDelta;

	vPos = XMVectorSetW(vPos, 1.f);
	vPos = XMVectorSetZ(vPos, fDepth);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	_float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vOriginPos));

	if (fDistance <= fEndDistance)
	{
		vTargetPos = XMVectorSetW(vTargetPos, 1.f);
		vTargetPos = XMVectorSetZ(vTargetPos, fDepth);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTargetPos);

		m_bCheck = TRUE;
		return TRUE;
	}
	return FALSE;
}

_vector CUIObject::GetOffsetPostion(_vector vPosition)
{
	//직교의 0,0좌표는 뷰포트 상에 중앙으로 설정되어 있기에 왼쪽위를 따로 설정

	vPosition = XMVectorSetX(vPosition, XMVectorGetX(vPosition) - g_iWinSizeX * 0.5f );
	vPosition = XMVectorSetY(vPosition,  - XMVectorGetY(vPosition) + g_iWinSizeY * 0.5f);

	return vPosition;
}

_bool CUIObject::Animation(_vector vStartPos ,_vector vTargetPos, _float fSpeed, _float fDepth,_float fTimeDelta, _float fEndDistance)
{
	_bool bFinishAnim = { FALSE };

	vStartPos = XMVectorSetX(vStartPos, XMVectorGetX(vStartPos) * m_vOffSetWinSize.x);
	vStartPos = XMVectorSetY(vStartPos, XMVectorGetY(vStartPos) * m_vOffSetWinSize.y);

	vTargetPos = XMVectorSetX(vTargetPos, XMVectorGetX(vTargetPos) * m_vOffSetWinSize.x);
	vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) * m_vOffSetWinSize.y);

	if (m_bCheck)
	{
		m_fAnimDelayTiemr += fTimeDelta;

		if (m_fAnimDelayTiemr >= 0.25f)
		{
			m_bCheck = FALSE;
			m_fAnimDelayTiemr = 0.f;
			m_bStart = FALSE;
			m_pUI_Manager->m_bChange[m_eLRPos] = FALSE;
		}
	}

	if (m_pUI_Manager->m_bChange[m_eLRPos] == TRUE)
	{
		if (m_eLRPos == LEFT)
		{
			m_vAnimStartPos = GetOffsetPostion(vStartPos);
			m_vAnimTargetPos = vTargetPos;
		}
		else if (m_eLRPos == RIGHT)
		{
			vStartPos= XMVectorSetX(vStartPos, g_iWinSizeX  - XMVectorGetX(vStartPos));
			m_vAnimStartPos = GetOffsetPostion(vStartPos);
			m_vAnimTargetPos = vTargetPos; 
		}

		if (m_bStart == FALSE && m_bCheck== FALSE)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vAnimStartPos);
			m_bStart = TRUE;
		}
		
		if(m_bStart)
			bFinishAnim = MoveAnimUI(vTargetPos, fSpeed, fDepth , fTimeDelta, fEndDistance);
	}

	return bFinishAnim;
}

HRESULT CUIObject::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIObject::Ready_Components()
{

	//MSG_BOX(TEXT("Debug1"));
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI_VtxRect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//MSG_BOX(TEXT("Debug2"));
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

void CUIObject::Free()
{


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pUI_Manager);

	__super::Free();
}
