//#pragma once
//
//#include "Model.h"
//#include "Bone_Data.h"
//BEGIN(Engine)
//
//class CBone final : public CBase
//{
//private:
//	CBone();
//	virtual ~CBone() = default;
//
//public:
//#ifdef _DEBUG
//	HRESULT Initialize(aiNode* pAINode, _int iParentIndex);
//#endif
//	HRESULT Initialize(BONE_DATA& BoneData);
//	void Invalidate_CombinedTransformationMatrix(CModel::BONES& Bones, _fmatrix PivotMatrix );
//
//public:
//	const _char* Get_Name() const {
//		return m_szName;
//	}
//
//	_matrix Get_CombinedTransformationMatrix() const {
//		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
//	}
//
//	_float4x4 Get_TransformationMatrix() const {
//		return m_TransformationMatrix;
//	}
//
//	void	Set_TransformationMatrix(_fmatrix TransformationMatrix) {
//		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
//	}
//
//	void	Set_IsRoot(_bool bValue) { m_bIsRoot = bValue; }
//	_bool	Get_IsRoot() { return m_bIsRoot; }
//
//private:
//	_char	m_szName[MAX_PATH] = "";
//	_int	m_iParentIndex = { 0 };
//
//	_float4x4	m_TransformationMatrix;
//	_float4x4	m_CombinedTransformationMatrix;
//
//	_bool		m_bIsRoot = { false };
//
//public:
//#ifdef _DEBUG
//	static CBone* Create(aiNode* pAINode, _int iParentIndex);
//#endif
//	static CBone* Create(BONE_DATA& BoneData);
//	CBone* CBone::Clone();
//	virtual void Free() override;
//};
//
//END
//
