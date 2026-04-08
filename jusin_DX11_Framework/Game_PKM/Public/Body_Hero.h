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
	typedef struct tagBodyPlayerDesc : public CPartObject::PARTOBJECT_DESC
	{
		const _uint* pParentState = { nullptr };
	}BODY_PLAYER_DESC;
private:
	CBody_Hero(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Hero(const CBody_Hero& Prototype);
	virtual ~CBody_Hero() = default;

public:
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	const _uint* m_pParentState = { nullptr };
	_uint m_iDummy = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	static CBody_Hero* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free();
};

NS_END