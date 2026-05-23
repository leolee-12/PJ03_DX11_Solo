#pragma once
#include "Game_PKM_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
NS_END

NS_BEGIN(Game_PKM)

class CVIBuffer_FieldGrass_Instance;

class CFieldGrassBatch final : public CGameObject
{
public:
	struct FIELDGRASS_BATCH_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		const _float3* pPositions = nullptr;
		_uint iNumPositions = 0;
	};

private:
	CFieldGrassBatch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFieldGrassBatch(const CFieldGrassBatch& Prototype);
	virtual ~CFieldGrassBatch() = default;

public:
	virtual _string Get_TypeName() const override { return "FieldGrassBatch"; }
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Build_Instances(const FIELDGRASS_BATCH_DESC& Desc);

private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_FieldGrass_Instance* m_pVIBufferCom = nullptr;

	vector<VTXFIELDGRASS_INSTANCE> m_Instances;

public:
	static CFieldGrassBatch* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END