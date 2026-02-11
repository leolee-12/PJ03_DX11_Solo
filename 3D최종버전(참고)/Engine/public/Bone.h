#pragma once

#include "Base.h"

/* assimp 뼈를 표현하는 데이터 타입 */
/* aiBone, aiNode, aiNodeAnim : 이 세개의 뼈 이름은 모두 동기화되어있다.  */

/* aiBone : 이 뼈가 어떤 정점에게 영향을 주는가? 얼마나? */
/* aiNode : 뼈. */
/* aiNodeAnim : 특정 애니메이션 안에서 이용되는 뼈의 정보(시간대별 상태) */

NS_BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	_matrix Get_CombinedTransformationMatrix() {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}
	const _float4x4* Get_CombinedTransformationMatrix_Ptr() {
		return &m_CombinedTransformationMatrix;
	}

public:
	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

public:
	HRESULT Initialize(const aiNode* pAINode, _int iParentIndex);
	_bool Compare_Name(const _char* pName) {
		return !strcmp(pName, m_szName);
	}
	void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformationMatrix);
private:
	_char				m_szName[MAX_PATH] = {};
	_float4x4			m_TransformationMatrix = {};
	_float4x4			m_CombinedTransformationMatrix = {};
	_int				m_iParentBoneIndex = { -1 };

public:
	static CBone* Create(const aiNode* pAINode, _int iParentIndex);
	CBone* Clone();
	virtual void Free() override;
};

NS_END