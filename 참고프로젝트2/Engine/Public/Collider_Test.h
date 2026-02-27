#pragma once
#include "Component.h"
//#include "VIBuffer.h"
#include "Transform.h"


#define MIN 3
#define MAX 5

//MIN 3, MAX 5  :기존 코드
//MIN 7 MAX 1 : 45도 회전 이후? //작동안함

BEGIN(Engine)
class ENGINE_DLL CCollider_Test : public CComponent// :    public CVIBuffer
{
public:
	typedef struct COLLIDER_DESC
	{
		_float3 Position;
		CTransform* pTransform;
		_float fSizeX;
		_float fSizeY;
		_float fSizeZ = { 0.5f };
		//_bool bObstacle = { false };

		//중점으로부터 Offset만큼 이동된 위치에 생성
		_float3 Offset = {};
	};


private:
	CCollider_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider_Test(const CCollider_Test& rhs);
	virtual ~CCollider_Test() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;	
	//virtual void Update();
	//virtual void Update(_float3 Offset);	// 디폴트 0.f 로 해서 하나만 만들까
	virtual void Update(_float3 Offset = { 0.f,0.f,0.f });
	virtual HRESULT Render(); 
	virtual HRESULT Render(_float4 fColor);

	 
	bool isColliding(const CCollider_Test* other);// const;
	bool isColliding(CComponent* other);// const;

	bool isCollidingAndAddCheck(CComponent* other);// const;
	bool GetnSwitch_AddCheck();
	void SetAddCheck(_bool bCheck)
	{
		m_bAddCollisionCheck = bCheck;
	}


	_float3 Add(_float3 Tmp, _float3 Dst)
	{
		return { Tmp.x + Dst.x, Tmp.y + Dst.y , Tmp.z + Dst.z };
	}



	static _float3 CalculateOverlapLengthsAIO(const XMFLOAT3& min1, const XMFLOAT3& max1, const XMFLOAT3& min2, const XMFLOAT3& max2);
	static _float3 CalculateOverlapLengthsAIO(CCollider_Test* pTmpCollider, CCollider_Test* pDstCollider);
	static _float3 CalculateOverlapLengths2(const XMFLOAT3& min1, const XMFLOAT3& max1, const XMFLOAT3& min2, const XMFLOAT3& max2);


	_float3 Get_Min()
	{
		return m_WorldVertices[MIN];
	}

	_float3 Get_Max()
	{
		return m_WorldVertices[MAX];
	}

	_float3 Get_Middle()
	{
		return  { 
			(m_WorldVertices[MIN].x + m_WorldVertices[MAX].x)*0.5f  ,  
			(m_WorldVertices[MIN].y + m_WorldVertices[MAX].y) * 0.5f, 
			(m_WorldVertices[MIN].z + m_WorldVertices[MAX].z) * 0.5f };
	}


private:
	CTransform* m_pTargetTransform;

	_float3 m_LocalVertices[8];
	_float3 m_WorldVertices[8];
	_bool m_bAddCollisionCheck = false;

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif

public:
	static CCollider_Test* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END