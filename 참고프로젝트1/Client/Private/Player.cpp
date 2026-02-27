#include "Player.h"

#include "Body.h"
#include "Weapon.h"

#include "GameInstance.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject { pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
    : CContainerObject{ Prototype }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
    CContainerObject::CONTAINER_OBJECT_DESC     Desc{};

    Desc.fSpeedPerSec = 10.f;
    Desc.fRotationPerSec = XMConvertToRadians(180.0f);
    Desc.iNumPartObjects = ENUM_CLASS(PARTOBJ::END);

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

  //   m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSet(0.f, 3.f, 0.f, 1.f));

    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    for (auto& pPartObj : m_PartObjects)
    {
        if(nullptr != pPartObj)
            pPartObj->Priority_Update(fTimeDelta);
    }
}

void CPlayer::Update(_float fTimeDelta)
{


    if (GetKeyState(VK_LEFT) & 0x8000)
    {
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
    }

    if (GetKeyState(VK_RIGHT) & 0x8000)
    {
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
    }

    if (GetKeyState(VK_DOWN) & 0x8000)
    {
        m_pTransformCom->Go_Backward(fTimeDelta);
    }

    if (GetKeyState(VK_UP) & 0x8000)
    {
        m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);

        if(m_iState & STATE::IDLE)
            m_iState ^= STATE::IDLE;

        m_iState |= STATE::RUN;
    }
    else
    {
        m_iState = STATE::IDLE;
    }

    if (GetKeyState(VK_LBUTTON) & 0x8000)
    {
        _float3     vPickPos;
        if (true == m_pGameInstance->Picking(&vPickPos))
            m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPickPos), 1.f));
    }

    //m_pTransformCom->Set_State(Engine::STATE::POSITION,
    //    m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(Engine::STATE::POSITION)));

    for (auto& pPartObj : m_PartObjects)
    {
        if (nullptr != pPartObj)
            pPartObj->Update(fTimeDelta);
    }

    m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));
}

void CPlayer::Late_Update(_float fTimeDelta)
{
    for (auto& pPartObj : m_PartObjects)
    {
        if (nullptr != pPartObj)
            pPartObj->Late_Update(fTimeDelta);
    }


    
#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
    m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
#endif
}

HRESULT CPlayer::Render()
{


	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
    /* For. Com_Navigation */
    CNavigation::NAVIGATION_DESC     NaviDesc{};
    NaviDesc.iCurrentCellIndex = 0;
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    /* For. Com_Collider */
    CBounding_OBB::OBB_DESC     ColliderDesc{};
    ColliderDesc.vSize = _float3(0.8f, 1.2f, 0.8f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
    ColliderDesc.vRadians = _float3(0.f, 0.f, 0.f);

   /* CBounding_AABB::AABB_DESC     ColliderDesc{};
    ColliderDesc.vSize = _float3(0.8f, 1.2f, 0.8f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);   
        */
    if (FAILED(Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
    CBody::BODY_DESC        BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BodyDesc.pParentState = &m_iState;
    if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player_Body"), ENUM_CLASS(PARTOBJ::BODY), &BodyDesc)))
        return E_FAIL;

    


    CWeapon::WEAPON_DESC        WeaponDesc{};
    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    WeaponDesc.pParentState = &m_iState;
    WeaponDesc.pSocketBoneMatrix = dynamic_cast<CBody*>(m_PartObjects[ENUM_CLASS(PARTOBJ::BODY)])->Get_SocketBoneMatrix_Ptr("SWORD");

    if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player_Weapon"), ENUM_CLASS(PARTOBJ::WEAPON), &WeaponDesc)))
        return E_FAIL;      

    return S_OK;
}

CGameObject* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer* pInstance = new CPlayer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}


CGameObject* CPlayer::Clone(void* pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CPlayer");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

    Safe_Release(m_pColliderCom);
    Safe_Release(m_pNavigationCom);
}
