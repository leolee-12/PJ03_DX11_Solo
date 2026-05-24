#pragma once
#include "Game_PKM_Defines.h"
#include "GameObject.h"
#include "Effect_Defines.h"
#include <deque>

NS_BEGIN(Engine)
class CShader;
class CTexture;
NS_END

NS_BEGIN(Game_PKM)
class CVIBuffer_Trail;

class CTrail final : public CGameObject
{
public:
	struct TRAIL_DESC final : public CGameObject::GAMEOBJECT_DESC
	{
		_uint   iMaxSegments = 32;
		_float  fSegmentSpacing = 0.05f;
		_float  fLifeTimePerSegment = 0.5f;
		_float  fWidthStart = 0.3f;
		_float  fWidthEnd = 0.0f;
		_float3 vUpAxis = { 0.f, 1.f, 0.f };
		BLEND_MODE eBlend = BLEND_MODE::ADDITIVE;
		_bool   bIgnoreDepth = false;
		WNameID strTextureProtoTag = PROTO_COM_TEX_DUMMY_WHITE;
		_uint   iTextureProtoLevel = ETOUI(LEVEL::STATIC);
		CCurveColor curveColor;
		CTransform* pParentTransform = nullptr;   // effect root
	};

private:
	CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail(const CTrail& Prototype);
	virtual ~CTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Priority_Update(_float fTimeDelta) override;
	virtual void    Update(_float fTimeDelta) override;
	virtual void    Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Push_Tip(const _float3& vWorldPos);   // 특수 케이스 외부 push
	void Stop() { m_bStopped = true; }          // 새 tip 중단 → 자연 소멸

private:
	HRESULT Ready_Components();
	void    Build_Ribbon();

private:
	struct SEGMENT { _float3 vPos; _float fAge; };

	TRAIL_DESC   m_tDesc = {};
	CTransform* m_pParentTransform = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_Trail* m_pVIBufferCom = nullptr;

	std::deque<SEGMENT> m_Segments;
	vector<VTXTRAIL>    m_Ribbon;
	_bool   m_bStopped = false;
	_bool   m_bHasLastTip = false;
	_float3 m_vLastTip = {};

public:
	static CTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
private:
	virtual void Free() override;
};
NS_END