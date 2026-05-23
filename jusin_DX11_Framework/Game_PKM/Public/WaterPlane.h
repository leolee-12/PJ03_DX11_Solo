#pragma once
#include "Game_PKM_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
NS_END

NS_BEGIN(Game_PKM)
class CVIBuffer_XZPlane;

class CWaterPlane final : public CGameObject
{
public:
	struct WATER_PLANE_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float fWidth = 80.f;
		_float fDepth = 80.f;
		_float fTileU = 4.f;
		_float fTileV = 4.f;
		_float fTimeScale = 1.f;
		_float fScale = 1.f;
	};

private:
	enum TEXTURETYPE { NET02, NORM, NET01, LIGHT, SKY, END };

private:
	CWaterPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWaterPlane(const CWaterPlane& Prototype);
	virtual ~CWaterPlane() = default;

public:
	virtual _string Get_TypeName() const override { return "WaterPlane"; }
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	WATER_PLANE_DESC m_tDesc = {};
	_float m_fTime = 0.f;

	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_XZPlane* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

public:
	static CWaterPlane* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END