#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CNavigation;
class CVIBuffer_Terrain;
NS_END

NS_BEGIN(Client)

class CTerrain final : public CGameObject
{
public:
	enum TEXTURETYPE { TEXTURE_DIFFUSE, TEXTURE_MASK, TEXTURE_BRUSH, TEXTURE_END };
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& Prototype);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom[TEXTURE_END] = {nullptr};
	CNavigation*		m_pNavigationCom = { nullptr };
	CVIBuffer_Terrain*	m_pVIBufferCom = { nullptr };	
	
private:
	HRESULT Ready_Components();

public:	
	static CGameObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);/* 盔屈积己 */
	virtual CGameObject* Clone(void* pArg) override;/* 荤夯积己 */ 
	virtual void Free();
};

NS_END