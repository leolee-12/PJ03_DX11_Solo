#pragma once

#include "Base.h"

/* assimp : 뼈를 표현 (aiNode*, aiBone*, aiNodeAnim*) */
/* 뼈의 상태를 표현하기위한 데이터를 보관한다. (aiNode) */
/* 뼈의 상속관계를 유지한다. (aiNode) */
/* 각각의 뼈가 어떤 정점에게 얼마나 영향을 주는지? (aiBone) */

/* 뼈의 시간에 따른 상태 변화에 대한 기능. */
/* 위의 상태들을 변수화하여 정점에게 전달하는 기능 */

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char* Get_Name() const {
		return m_szName;
	}

	const _float4x4* Get_TransformationMatrix() const {
		return &m_TransformationMatrix;
	}

	const _float4x4* Get_CombinedTransformationMatrix() const {
		return &m_CombinedTransformationMatrix;
	}

public:
	void SetUp_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

public:
	HRESULT Initialize(const string& name, _int parentIndex, const XMFLOAT4X4& transformationMatrix);

public:
	bool IsDummyBone() const { return m_isDummyBone; }
	void SetDummyBone(bool isDummy) { m_isDummyBone = isDummy; }

	void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);
	
	
	//_matrix			ChildMatrix = XMMatrixTranslation(10, 0, 0);
	//_matrix			ParentMatrix = XMMatrixTranslation(10, 10, 10);

	// ParentMatrix = ParentMatrix.TransformationMatrix * GransFather`s MAtrix;

	//_matrix			BoneMatrix = ChildMatrix * ParentMatrix;

public:
	_char				m_szName[MAX_PATH] = {};

	/* 내 뼈만의 상태행렬(자식행렬) => 부모 기준으로 표현된 상태행렬. */
	_float4x4			m_TransformationMatrix = {};
	_float4x4			m_CombinedTransformationMatrix = {};

	/* 추후 뼈를 복제할 때 나는 깊은 복사로 처리할려고 한다. */
	/* CBone객체도 복제를 통해 생성하게 될 것이고 본이 가지고 있는 부모본의 주소를 새롭게 생성한 본의 주소로 모두 교체해주어야한다. xxxxxxxxxxx */ 
	// CBone*				m_pParent = { nullptr };
	/* 추후 뼈를 복제할 때 나는 깊은 복사로 처리할려고 한다. */
	/* CBone객체도 복제를 통해 생성하게 될 것이고 본이 가지고 있는 부모본의 인덱스또한 걍 복제되면 된다. */
	_int				m_iParentIndex = { -1 };
	_bool				m_isDummyBone = { false };
public:
	static CBone* Create(const string& name, _int parentIndex, const XMFLOAT4X4& transformationMatrix);

	CBone* Clone();
	virtual void Free() override;
};

END