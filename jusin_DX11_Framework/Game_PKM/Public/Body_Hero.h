#pragma once
#include "Game_PKM_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Game_PKM)

class CBody_Hero final : public CPartObject
{
public:
	struct BODY_HERO_DESC : public CPartObject::PARTOBJECT_DESC
	{
		const _uint* pParentState = { nullptr };
	};

	enum MATERIAL_NAME { BAG, BOTTOMS, CAP, R_EYE, L_EYE, SKIN, HAIR, SHOES, TOPS, END };

private:
	CBody_Hero(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Hero(const CBody_Hero& Prototype);
	virtual ~CBody_Hero() = default;

public:
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;
	void Set_Anim(_uint iAnimIdx, _bool isLoop = false, _float fBlendDuration = DEFAULT_BLENDDURATION);
	void Set_Variant(unsigned int iMatIdx, MATERIAL_TYPE eType, unsigned int iMatNum) { m_RenderTable.variants[iMatIdx][static_cast<unsigned int>(eType)] = iMatNum; }
	void Set_Pass(unsigned int iMatIdx, unsigned int iPassIdx) { m_RenderTable.passes[iMatIdx] = iPassIdx; }
	const _float3& Get_RootMotionDelta() const;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	const _uint* m_pParentState = { nullptr };
	_uint m_iDummy = {};
	RENDER_TABLE m_RenderTable;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void Ready_DefaultVariant();

public:
	static CBody_Hero* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free();
};

NS_END