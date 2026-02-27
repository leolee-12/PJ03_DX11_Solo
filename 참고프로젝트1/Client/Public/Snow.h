#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect_Instancing;
NS_END

NS_BEGIN(Client)

class CSnow final : public CGameObject
{
private:
	CSnow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSnow(const CSnow& Prototype);
	virtual ~CSnow() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	CVIBuffer_Rect_Instancing*		m_pVIBufferCom = { nullptr };	
	
private:
	HRESULT Ready_Components();

public:	
	static CGameObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);/* 盔屈积己 */
	virtual CGameObject* Clone(void* pArg) override;/* 荤夯积己 */ 
	virtual void Free();
};

NS_END