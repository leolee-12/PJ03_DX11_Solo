#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	_bool Compare_Name(const _char* pBoneName) { return !_stricmp(pBoneName, m_szName); }
	_int Get_ParentIndex() { return m_iParentIndex; }
	const _float4x4& Get_BindPoseMatrix() const { return m_BindPoseMatrix; }
	const _float4x4& Get_TransformationMatrix() const { return m_TransformationMatrix; }
	_float4x4& Get_TransformationMatrix() { return m_TransformationMatrix; }
	const _float4x4* Get_CombinedTransformationMatrixPtr() const { return &m_CombinedTransformationMatrix; }
	void XM_CALLCONV Set_TransformationMatrix(_fmatrix TransformationMatrix) { XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix); }
	void Zero_TranslationXZ() { m_TransformationMatrix._41 = 0.f; m_TransformationMatrix._43 = 0.f; }

	HRESULT Initialize(const WMODEL_BONE& tBone);
	void Update_CombinedTransformMatrices(const vector<class CBone*>& Bones);
	void Decompose_BindPose(BONE_SRT& outSnapshot) const;
	void Decompose_Transformation(BONE_SRT& outSnapshot) const;
	void Decompose_Combined(BONE_SRT& outSnapshot) const;

private:
	_char m_szName[MAX_PATH] = {};
	_int m_iParentIndex = { -1 };
	_float4x4 m_BindPoseMatrix = {};
	_float4x4 m_TransformationMatrix = {};
	_float4x4 m_CombinedTransformationMatrix = {};

public:
	static CBone* Create(const WMODEL_BONE& tBone);
	CBone* Clone();
private:
	virtual void Free() override;
};

NS_END