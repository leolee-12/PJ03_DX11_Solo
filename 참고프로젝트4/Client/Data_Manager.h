#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Model_Data.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
END

BEGIN(Client)
class CData_Manager :   public CBase
{
public:
	typedef struct tagDebugDesc
	{
		_int		iInt;
		_float		fFloat;
		_bool		bBool;
		string		strString;
		wstring		wstrString;
		_float3	    vFloat3;
		_float3	    vFloat3_2;
		_float4x4	matWorld;
	}DEBUGDESC;

public:
	DECLARE_SINGLETON(CData_Manager)

private:
	CData_Manager();
	virtual	~CData_Manager() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);

public:	
	HRESULT	Save_ObjectData(string strPath, vector<shared_ptr<CGameObject>>* pObjectList, LAYERTYPE eType);
	//HRESULT	Save_ObjectData(string strPath, shared_ptr<CGameObject> pObject);
	HRESULT	Save_ObjectData(string strPath, weak_ptr<CGameObject> pObject);
	HRESULT	Load_ObjectData(string strPath, LEVEL eLevel,LAYERTYPE eType);

	HRESULT	Load_LaserData(string strPath,vector<_float4>* vecPos);

	_bool Load_BinaryData(string strPath, MODEL_DATA& pModelData);

	DEBUGDESC Load_DebugJson();

public:
	HRESULT	Save_InstanceData(string strPath, vector<shared_ptr<CGameObject>> pObjectList);
	HRESULT Load_InstanceData(string strPath, LEVEL eLevel);

private:
	class CGameInstance* m_pGameInstance = { nullptr };	
	//vector< CGameObject*>* m_pGameObjectList[LAYERTYPE_END];

public:
	virtual void	Free() override;
};
END
