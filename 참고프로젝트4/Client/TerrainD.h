#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_TerrainD;
END

BEGIN(Client)

class CTerrainD final : public CGameObject
{
	INFO_CLASS(CTerrainD, CGameObject)
public:
	typedef struct tagTerrainDDesc : public tagGameObjectDesc
	{
		_uint iVerticesXNum = 0;
		_uint iVerticesZNum = 0;
	} TERRAIND_DESC;

	enum TEXTURE { TYPE_DIFFUSE, TYPE_MASK, TYPE_BRUSH, TYPE_END };
	
private:
	CTerrainD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrainD(const CTerrainD& rhs);
	virtual ~CTerrainD() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pTextureCom[TYPE_END] = { nullptr };

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	/* 원형객체를 생성한다. */
	static shared_ptr<CTerrainD> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual shared_ptr<CGameObject> Clone(void* pArg) override;

	virtual void Free() override;
};

END