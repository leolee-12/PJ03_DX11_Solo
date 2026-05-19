#include "VP_PickingCtrl.h"
#include "VP_CoordMapper.h"
#include "EditInstance.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Model.h"
#include "UIEditorSession.h"

CVP_PickingCtrl::CVP_PickingCtrl()
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_pEditInstance(CEditInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pEditInstance);
}

HRESULT CVP_PickingCtrl::Initialize(CVP_CoordMapper* pMapper)
{
	if (nullptr == pMapper)
		return E_FAIL;

	m_pMapper = pMapper;
	Safe_AddRef(m_pMapper);

	return S_OK;
}

void CVP_PickingCtrl::Handle_DebugPicking()
{
	if (m_pEditInstance->Get_UISession()->Get_VPMode()
		!= CUIEditorSession::VPMODE::SCENE) return;

	_float3 vRayOriginF3{}, vRayDirF3{};
	if (false == Build_MouseRay(&vRayOriginF3, &vRayDirF3))
	{
		m_bHasLastHit = false;
		m_strPickDebug = "Pick Failed : Ray Build Failed";
		m_strPickTarget = "Target : None";
		return;
	}

	XMVECTOR vRayOrigin = XMLoadFloat3(&vRayOriginF3);
	XMVECTOR vRayDir = XMVector3Normalize(XMLoadFloat3(&vRayDirF3));

	const vector<EDITOR_OBJECT_ENTRY>& Entries = m_pEditInstance->Get_EditorObjectEntries();

	_bool bHit = false;
	_float fMinDist = FLT_MAX;
	_float3 vBestLocalHit{};
	_float3 vBestWorldHit{};
	_float3 vBestObjectPos{};
	_string strBestTarget = "Target : None";

	for (const EDITOR_OBJECT_ENTRY& tEntry : Entries)
	{
		if (m_pEditInstance->Is_PlaceMode() || m_pEditInstance->Is_NavEditMode())
		{
			if (false == tEntry.bPlacementSurface)
				continue;
		}
		else
		{
			if (false == tEntry.bSelectable)
				continue;
		}

		CGameObject* pObj = tEntry.pObj;
		CModel* pModel = tEntry.pModel;

		if (nullptr == pObj || nullptr == pModel)
			continue;

		CTransform* pTransform = pObj->Get_Transform();
		if (nullptr == pTransform)
			continue;

		_float3 vLocalHitPos{};
		_float3 vWorldHitPos{};

		if (Pick_ModelObject(pObj, pModel, vRayOrigin, vRayDir, &vLocalHitPos, &vWorldHitPos))
		{
			const _float fHitDist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vWorldHitPos) - vRayOrigin));

			if (fHitDist < fMinDist)
			{
				fMinDist = fHitDist;
				bHit = true;

				vBestLocalHit = vLocalHitPos;
				vBestWorldHit = vWorldHitPos;

				_vector vObjPos = pTransform->Get_State(STATE::POSITION);
				XMStoreFloat3(&vBestObjectPos, vObjPos);

				strBestTarget = "Target : " + WtoS(pObj->Get_Name());
				m_pLastPickedObject = pObj;
			}
		}
	}

	if (!bHit)
	{
		m_bHasLastHit = false;
		m_pLastPickedObject = nullptr;
		m_strPickDebug = "Pick Result : No Hit";
		m_strPickTarget = "Target : None";
		return;
	}

	m_bHasLastHit = true;
	m_vLastObjectPos = vBestObjectPos;
	m_vLastLocalHitPos = vBestLocalHit;
	m_vLastWorldHitPos = vBestWorldHit;
	m_strPickDebug = "Pick Result : Hit";
	m_strPickTarget = strBestTarget;
}

void CVP_PickingCtrl::Handle_ViewportClick()
{
	if (m_pEditInstance->Get_UISession()->Get_VPMode()
		!= CUIEditorSession::VPMODE::SCENE) return;

	Handle_DebugPicking();
	if (!m_bHasLastHit)
		return;

	// Nav 점 찍기 모드 - EditInstance를 통해 MapTool에 전달
	if (m_pEditInstance->Is_NavEditMode())
	{
		m_pEditInstance->Fire_NavClick(m_vLastWorldHitPos);
		return;
	}

	if (m_pEditInstance->Is_PlaceMode())
		Place_ObjectAtHit(m_vLastWorldHitPos);
	else
		Pick_SelectObject();
}

_bool CVP_PickingCtrl::Build_MouseRay(_float3* pOutOrigin, _float3* pOutDir) const
{
	if (nullptr == pOutOrigin || nullptr == pOutDir)
		return false;

	ImVec2 vMouse = ImGui::GetMousePos();
	const ImVec2& vDisplayPos = m_pMapper ? m_pMapper->Get_DisplayPos() : ImVec2(0.f, 0.f);
	const ImVec2& vDisplaySize = m_pMapper ? m_pMapper->Get_DisplaySize() : ImVec2(1.f, 1.f);

	const _float fLocalX = vMouse.x - vDisplayPos.x;
	const _float fLocalY = vMouse.y - vDisplayPos.y;

	if (fLocalX < 0.f || fLocalY < 0.f || fLocalX > vDisplaySize.x || fLocalY > vDisplaySize.y)
		return false;

	const _float fNdcX = (2.f * fLocalX / vDisplaySize.x) - 1.f;
	const _float fNdcY = 1.f - (2.f * fLocalY / vDisplaySize.y);

	const _float4x4* pProjInv = m_pGameInstance->Get_Transform_Inverse(D3DTS::PROJ);
	const _float4x4* pViewInv = m_pGameInstance->Get_Transform_Inverse(D3DTS::VIEW);

	if (nullptr == pProjInv || nullptr == pViewInv)
		return false;

	_vector vNear = XMVector3TransformCoord(
		XMVectorSet(fNdcX, fNdcY, 0.f, 1.f),
		XMLoadFloat4x4(pProjInv));

	_vector vFar = XMVector3TransformCoord(
		XMVectorSet(fNdcX, fNdcY, 1.f, 1.f),
		XMLoadFloat4x4(pProjInv));

	_matrix matViewInv = XMLoadFloat4x4(pViewInv);

	_vector vWorldNear = XMVector3TransformCoord(vNear, matViewInv);
	_vector vWorldFar = XMVector3TransformCoord(vFar, matViewInv);

	XMStoreFloat3(pOutOrigin, vWorldNear);

	XMVECTOR vWorldDir = XMVector3Normalize(vWorldFar - vWorldNear);
	XMStoreFloat3(pOutDir, vWorldDir);

	return true;
}

_bool CVP_PickingCtrl::Pick_ModelObject(CGameObject* pObj, CModel* pModel, _fvector vRayOrigin, _fvector vRayDir, _float3* pOutLocalHitPos, _float3* pOutWorldHitPos) const
{
	if (nullptr == pObj || nullptr == pModel || nullptr == pOutLocalHitPos || nullptr == pOutWorldHitPos)
		return false;

	CTransform* pTransform = pObj->Get_Transform();
	if (nullptr == pTransform)
		return false;

	XMMATRIX matWorld = XMLoadFloat4x4(pTransform->Get_WorldMatrixPtr());

	_bool bHit = false;
	_float fMinDist = FLT_MAX;

	for (_uint i = 0; i < static_cast<_uint>(pModel->Get_NumMeshes()); ++i)
	{
		CMesh* pMesh = pModel->Get_Mesh(i);
		if (nullptr == pMesh)
			continue;

		BoundingBox worldAABB;
		pMesh->Get_LocalAABB().Transform(worldAABB, matWorld);

		_float fAABBDist = 0.f;
		if (!worldAABB.Intersects(vRayOrigin, vRayDir, fAABBDist))
			continue;

		const auto& Positions = pMesh->Get_Positions();
		const auto& Indices = pMesh->Get_Indices();

		if (Positions.empty() || Indices.empty())
			continue;

		XMMATRIX matWorldInv = XMMatrixInverse(nullptr, matWorld);
		XMVECTOR vLocalOrigin = XMVector3TransformCoord(vRayOrigin, matWorldInv);
		XMVECTOR vLocalDir = XMVector3Normalize(XMVector3TransformNormal(vRayDir, matWorldInv));

		for (_uint j = 0; j < static_cast<_uint>(Indices.size()); j += 3)
		{
			XMVECTOR v0 = XMLoadFloat3(&Positions[Indices[j]]);
			XMVECTOR v1 = XMLoadFloat3(&Positions[Indices[j + 1]]);
			XMVECTOR v2 = XMLoadFloat3(&Positions[Indices[j + 2]]);

			_float fDist = 0.f;
			if (TriangleTests::Intersects(vLocalOrigin, vLocalDir, v0, v1, v2, fDist))
			{
				if (fDist < fMinDist)
				{
					fMinDist = fDist;

					XMVECTOR vLocalHit = vLocalOrigin + vLocalDir * fDist;
					XMVECTOR vWorldHit = XMVector3TransformCoord(vLocalHit, matWorld);

					XMStoreFloat3(pOutLocalHitPos, vLocalHit);
					XMStoreFloat3(pOutWorldHitPos, vWorldHit);

					bHit = true;
				}
			}
		}
	}

	return bHit;
}

void CVP_PickingCtrl::Place_ObjectAtHit(const _float3& vHitPos)
{
	if (false == m_pEditInstance->Is_PlaceMode())
		return;

	const CATALOG_ITEM& tItem = m_pEditInstance->Get_PlaceItem();

	m_pEditInstance->Register_Object(
		tItem.iProtoLevel,
		tItem.strProtoTag,
		tItem.iLayerLevel,
		tItem.strLayerTag,
		nullptr);

	const auto& EditorObjects = m_pEditInstance->Get_EditorObjects();
	if (EditorObjects.empty())
		return;

	CGameObject* pNewObj = EditorObjects.back();
	if (nullptr == pNewObj)
		return;

	CTransform* pTransform = pNewObj->Get_Transform();
	if (nullptr == pTransform)
		return;

	pTransform->Set_State(STATE::POSITION, XMVectorSet(vHitPos.x, vHitPos.y, vHitPos.z, 1.f));
	m_pEditInstance->Select(pNewObj, false);
}

void CVP_PickingCtrl::Pick_SelectObject()
{
	if (nullptr == m_pLastPickedObject)
		return;

	m_pEditInstance->Select(m_pLastPickedObject, false);
}

CVP_PickingCtrl* CVP_PickingCtrl::Create(CVP_CoordMapper* pMapper)
{
	CVP_PickingCtrl* pInstance = new CVP_PickingCtrl();

	if (FAILED(pInstance->Initialize(pMapper)))
	{
		MSG_BOX("Failed to Create : CVP_PickingCtrl");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVP_PickingCtrl::Free()
{
	__super::Free();

	Safe_Release(m_pMapper);
	Safe_Release(m_pEditInstance);
	Safe_Release(m_pGameInstance);
}