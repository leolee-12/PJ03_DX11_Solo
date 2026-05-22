#pragma once
#include "Game_PKM_Defines.h"
#include "GameObject.h"
#include "Effect_Defines.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Game_PKM)

class CEffect_Mesh final : public CGameObject
{
public:
	struct MESH_EFFECT_DESC final : public CGameObject::GAMEOBJECT_DESC
	{
		WNameID  strModelProtoTag = { INVALID_TAG };
		WNameID  strShaderProtoTag = { INVALID_TAG };
		WNameID  strTextureProtoTag = PROTO_COM_TEX_DUMMY_WHITE;
		_uint    iTextureProtoLevel = ETOUI(LEVEL::STATIC);

		BLEND_MODE eBlend = BLEND_MODE::ADDITIVE;
		_bool    bIgnoreDepth = false;

		MESH_EFFECT_DEFINITION::SCALE_AXIS eScaleAxis =
			MESH_EFFECT_DEFINITION::SCALE_AXIS::Z_ONLY;

		CCurveFloat curveScale;
		CCurveColor curveColor;
		CCurveFloat curveAlpha;
		_float   fLifeTime = 1.f;

		CTransform* pParentTransform = nullptr;
	};

private:
	CEffect_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Mesh(const CEffect_Mesh& Prototype);
	virtual ~CEffect_Mesh() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Priority_Update(_float fTimeDelta) override;
	virtual void    Update(_float fTimeDelta) override;
	virtual void    Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderGlobals();

private:
	MESH_EFFECT_DESC m_tDesc = {};
	_float           m_fAge = 0.f;
	CTransform* m_pParentTransform = nullptr;

	CModel* m_pModelCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;

	_float3 m_vCurrentScale = { 1.f, 1.f, 1.f };
	_float4 m_vCurrentColor = { 1.f, 1.f, 1.f, 1.f };

public:
	static CEffect_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END