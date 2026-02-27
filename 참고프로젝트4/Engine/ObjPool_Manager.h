#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CGameInstance;

class ENGINE_DLL CObjPool_Manager : public CBase
{
	DECLARE_SINGLETON(CObjPool_Manager)

private:
	CObjPool_Manager();
	virtual ~CObjPool_Manager() = default;

public:
	HRESULT		Initialize(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

	void	Ready_Pool();

	//반환 타입 템플릿으로
	template<class T1>
	shared_ptr<T1> Create_Object(wstring strProtoTag, LAYERTYPE eLayer, void* pArg = nullptr, shared_ptr< CGameObject> pOwner = nullptr, _float3 vSpawnPos = {0.f,0.f,0.f},_float3 vScale = {1.f,1.f,1.f});
	
	//풀링용 오브젝트 생성
	HRESULT			Add_ObjectPool(wstring strTag, _uint iMaxNum = 50);

	//풀링용 오브젝트 Dead시 반환
	void			Return_Object(shared_ptr<CGameObject> pGameObject);

	queue<shared_ptr<class CGameObject>>* Find_ObjectPool(wstring typeName);

private:
	unordered_map<wstring, queue<shared_ptr< CGameObject>>> m_ObjectPool;

	ComPtr<ID3D11Device> m_pDevice = nullptr;
	ComPtr<ID3D11DeviceContext> m_pContext = nullptr;
	class CGameInstance* m_pGameInstance = nullptr;

public:
	virtual void Free() override;
};

template<class T1>
inline shared_ptr<T1> CObjPool_Manager::Create_Object(wstring strProtoTag, LAYERTYPE eLayer, void* pArg, shared_ptr<class CGameObject> pOwner, _float3 vSpawnPos,  _float3 vScale)
{
	queue<shared_ptr<CGameObject>>* pObjectPool = Find_ObjectPool(strProtoTag);
	if (pObjectPool != nullptr)
	{
		shared_ptr<T1> pObject = static_pointer_cast<T1>(pObjectPool->front());
		//pObject->Initialize(pArg);
		pObject->TurnOn_State(OBJSTATE::Active);
		pObject->Set_Owner(pOwner);
		
		weak_ptr<CTransform> pTransformCom =  pObject->Get_TransformCom();
		pTransformCom.lock()->Set_State(CTransform::STATE::STATE_POSITION, vSpawnPos);
		pTransformCom.lock()->Set_Scaling(vScale.x,vScale.y,vScale.z);
		
		m_pGameInstance->Add_Object(m_pGameInstance->Get_CreateLevelIndex(), eLayer, pObject);
		pObjectPool->pop();

		return pObject;
	}
	return nullptr;

}

/*template<class T1>
inline void CObjPool_Manager::Add_Object()
{
	if (is_base_of<CBase, T1>::value)
	{
		for (_uint i = 0; i < iMaxObject; ++i)
		{
			shared_ptr<CGameObject> pObject = T1::Create(m_pDevice, m_pContext);
			pObject->Set_IsPoolObject(true);
			pObject->TurnOff_State(OBJSTATE::Active);

			auto typeName = typeid(T1).name();
			if (m_ObjectPool.count(typeName) == 0)
			{
				queue<shared_ptr<CGameObject>> TempQueue;
				TempQueue.push(pObject);
				m_ObjectPool.insert({ typeName, TempQueue });
			}
			else
			{
				m_ObjectPool[typeName].push(pObject);
			}
		}		
	}
}*/

END
