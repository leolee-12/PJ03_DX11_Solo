#pragma once
#include "Game_PKM_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Game_PKM)

class CBody_BattleBase abstract : public CPartObject
{
public:
	struct BODY_BATTLE_DESC : public CPartObject::PARTOBJECT_DESC
	{
		WNameID strModelProtoTag = {};
		WNameID strShaderProtoTag = {};
		_uint iDefaultAnim = { 0 };
		_bool bLoop = { true };
		_float fScale = { 1.f };
		_bool bEnableRootMotion = { false };
		_uint iRootMotionBoneIndex = { 0 };
		const _uint* pParentState = { nullptr };
	};

protected:
	CBody_BattleBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_BattleBase(const CBody_BattleBase& Prototype);
	virtual ~CBody_BattleBase() = default;

public:
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;
	const _float3& Get_RootMotionDelta() const;
	void Set_Anim(_uint iAnimIdx, _bool isLoop = false, _float fBlendDuration = g_kDefaultBlendDuration);

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

protected:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	WNameID m_strModelProtoTag = {};
	WNameID m_strShaderProtoTag = {};
	const _uint* m_pParentState = { nullptr };

protected:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources_Common();

public:
	virtual CGameObject* Clone(void* pArg) PURE;

protected:
	virtual void Free() override;
};

NS_END