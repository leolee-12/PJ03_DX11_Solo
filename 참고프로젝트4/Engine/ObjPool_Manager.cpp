#include "ObjPool_Manager.h"
#include "GameInstance.h"
#include "BlendObject.h"

IMPLEMENT_SINGLETON(CObjPool_Manager)

CObjPool_Manager::CObjPool_Manager()
{
}

HRESULT CObjPool_Manager::Initialize(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    m_pDevice = pDevice;
    m_pContext = pContext;
    m_pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(m_pGameInstance);

    //Ready_Pool();

	return S_OK;
}

void CObjPool_Manager::Ready_Pool()
{
}   

HRESULT CObjPool_Manager::Add_ObjectPool(wstring strTag, _uint iMaxNum)
{	
	for (_uint i = 0; i < iMaxNum; ++i)
	{
		CGameObject::GAMEOBJECT_DESC Desc = {};
		Desc.bIsCloneInPool = false;
		shared_ptr<CGameObject> pObject = m_pGameInstance->CloneObject(strTag, &Desc);
		if (pObject == nullptr)
			RETURN_EFAIL;
		pObject->Set_IsPoolObject(true);
		pObject->TurnOff_State(OBJSTATE::Active);

		if (m_ObjectPool.count(strTag) == 0)
		{
			queue<shared_ptr<CGameObject>> TempQueue;
			TempQueue.push(pObject);
			m_ObjectPool.insert({ strTag, TempQueue });
		}
		else
		{
			m_ObjectPool[strTag].push(pObject);
		}
	}
	return S_OK;
}

void CObjPool_Manager::Return_Object(shared_ptr<CGameObject> pGameObject)
{
    const wstring strProtoTag = pGameObject->Get_PrototypeTag();
	/*if(strProtoTag == L"Player_BulletHit(Jiho)")
		_int a = 0;*/

    queue<shared_ptr<CGameObject>>* pObjectPool= Find_ObjectPool(strProtoTag);
	if (pObjectPool != nullptr)
	{
		pGameObject->TurnOff_State(OBJSTATE::Active);
		pGameObject->TurnOff_State(OBJSTATE::WillRemoved);

		if (dynamic_pointer_cast<CBlendObject>(pGameObject))
		{
			dynamic_pointer_cast<CBlendObject>(pGameObject)->Reset_Effect();
		}

		pObjectPool->push(pGameObject);
	}
}

queue<shared_ptr<class CGameObject>>* CObjPool_Manager::Find_ObjectPool(wstring typeName)
{
    if (m_ObjectPool.count(typeName) != 0)
    {
        return &m_ObjectPool[typeName];
    }
    return nullptr;
}


void CObjPool_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}
