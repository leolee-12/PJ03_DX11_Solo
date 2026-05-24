#pragma once
#include "PartObject.h"
#include "Game_PKM_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Game_PKM)

class CBattle_Ball final : public CPartObject
{
public:
	struct BATTLE_BALL_DESC : public CPartObject::PARTOBJECT_DESC
	{
		_float fScale = { 0.5f };
		_float3 vLocalOffset = {-0.1f, -0.05f, 0.0f};
	};

private:
	CBattle_Ball(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBattle_Ball(const CBattle_Ball& Prototype);
	virtual ~CBattle_Ball() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

	virtual _string Get_TypeName() const override { return "Battle_Ball"; }
	_float Get_ModelScale() const { return m_fScale; }

#ifdef _DEBUG
	void Set_DebugHoldVisible(_bool bHold) { m_bDebugHoldVisible = bHold; }
#endif

	void Show();
	void Hide();
	void Set_LocalOffset(const _float3& vLocalOffset);
	_bool Is_Visible() const { return m_bVisible; }

	_bool Set_Anim(_uint iAnimIdx, _bool isLoop = false, _float fBlendDuration = 0.f);
	_bool Was_AnimFinishedThisFrame() const { return m_bAnimFinishedThisFrame; }

	_uint Get_NumAnims() const;
	_uint Get_CurrAnim() const;

	void XM_CALLCONV Set_RotationCorrection(_fmatrix RotationMatrix);
	void Clear_RotationCorrection();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	const _float4x4* m_pSocketBoneMatrix = { nullptr };
	_bool m_bVisible = { false };
	_bool m_bAnimFinishedThisFrame = { false };
	_float4x4 m_RotationCorrection = {};
	_float m_fScale = { 0.5f };

#ifdef _DEBUG
	_bool m_bDebugHoldVisible = { false };
#endif

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_ShadowResources();

public:
	static CBattle_Ball* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END