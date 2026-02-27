#pragma once

#include "Base.h"
#include "Renderer_Defines.h"

/* 1. 화면에 그려야할 객체들을 그리는 순서대로 보관한다. */
/* 2. 보관하고 있는 객체들의 렌더함수를 호출한다.(렌더콜) */

BEGIN(Engine)
class CGameObject;
class CShader;
class CVIBuffer_Rect;
class CTransform;
class CGameInstance;
class CComponent;
END

BEGIN(Renderer)

class CRenderer final : public CBase
{
public:
						/* 맵은 프리올리티로 바로 그릴것 디퍼드(빛연산)이 필요한 애들은 논블렌드 */

	enum RENDERGROUP {
		RG_PRIORITY, RG_NONBLEND_TEST, RG_NONBLEND_LAYER, RG_GLOW_PRI, RG_BLEND_PRI, RG_GLOW_STAR, RG_SPACEMAP, RG_MAP, RG_STAGE,
		RG_PLAYER_METALLIC, RG_PLAYER_PART,
		RG_NONBLEND, RG_PLAYER, RG_SHADOWOBJ, RG_BACKSIDE_EFFECT,
		RG_NONLIGHT, RG_NONLIGHT_EFFECT, RG_GLOW, RG_BLEND, RG_UI, RG_UI_GLOW, RG_MULTY_GLOW,
		RG_CUTSCENE_PRI_EFFECT, RG_CUTSCENE_OBJECT, RG_CUTSCENE_LATE_EFFECT, RG_NODE, RG_END
	};

	enum MAP_TYPE { MAP_SPACE, MAP_VOLCANO, MAP_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject, RENDER_OBJECT* pDesc);
	HRESULT Add_Render_GlowDesc(RENDERGROUP eRenderGroup, GLOW_DESC* pDesc);
	HRESULT Add_DebugComponent(class CComponent* pDebugComponent);
	HRESULT Draw(_float fTimeDelta);
	void SetActive_RenderTarget(_bool isOn) { m_bShow_RenderTarget = isOn; };
	void SetActive_Debug_Component(_bool isOn) { m_bShow_Debug_Component = isOn; };

public:
	void Show_OutLine() { m_isOutLine = !m_isOutLine; }
	void Show_Layer_View() { m_isLayerView = !m_isLayerView; }
	_bool Get_isLayerView() { return m_isLayerView; }

private:
	_bool m_isOutLine = { false };

	_bool m_isLayerView = { true };
private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	list<class CGameObject*>					m_RenderObjects[RG_END];
	vector<string>								m_PlayerStrNames;
	list<GLOW_DESC>			m_GlowDescs[GLOW_END];
	
	list<class CComponent*>		m_DebugComponent;

	class CRenderInstance* m_pRenderInstance = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	class CShader* m_pShader = { nullptr };
	class CShader* m_pGlowShader = { nullptr };
	class CShader* m_pUI_GlowShader = { nullptr };
	class CVIBuffer_Rect* m_pVIBuffer = { nullptr };

	ID3D11DepthStencilView* m_pShadowDSV = { nullptr };

	_float4x4					m_DownWorldMatrix = {};
	_float4x4					m_DownWorldMatrix_Second = {};

	_float4x4					m_WorldMatrix = {};
	_float4x4					m_ViewMatrix = {};
	_float4x4					m_ProjMatrix = {};

	_bool m_bShow_RenderTarget = { false };
	_bool m_bShow_Debug_Component = { false };

	_uint m_iEffectRenderCount = { 0 };
	_uint m_iEffectGlowPri_RenderCount = { 0 };

private: /* For.BlackOut Variable */
	_bool m_isStartBlackOut = { false };
	_float m_fAccBlackTime = { 0.f };
	const _float m_fBlackTime = { 0.8f };
	

	_bool m_isStartAllBlackOut = { false };
	_float m_fAccAllBlackTime = { 0.f };
	_bool m_isDownAllBlack = { false };
	const _float m_fAllBlackTime = { 0.8f };
private:
	_bool m_isStartAllWhiteOut = { false };
	_float m_fAccAllWhiteTime = { 0.f };
	_bool m_isDownAllWhite = { false };
	_float m_fDurationTime = { 0.f };
	_float m_fAllWhiteSpeed = { 0.f };
private: /* For.WhiteOut Variable */
	_float m_fWhiteSpeed = { 1.f };
	_bool m_isStartWhiteOut = { false };
	_bool m_isEndWhiteOut = { false };
	_bool m_isMaintainWhite = { false };
	_float m_fAccWhiteTime = { 0.f };
	const _float m_fWhiteTime = { 1.f };
	_float m_fAccRockTime = { 0.f };
	_bool m_isRockStart = { false };
	_float2 m_fSpriteSize = { 0.f,0.f };
	_float2 m_fSpriteCurPos = { 0.f,0.f };
	_float m_fSpriteAccTime = { 0.f };
	_bool m_isSpriteEnd = { false };

	_float2 m_vWhiteDir = { 1.f,0.f };
	_bool* m_pDoneCheck = { nullptr };
	class CTexture* m_pEastFinish_TextureCom = { nullptr };
public: /* For.Distortion Function*/
	void Create_Distortion(DISTORTION_DESC& tDistortionDesc);
	void Delete_LoopDistortion();
	void Create_HitDistortion(_float4 vPlayerPos, _float3 vDir = { 1,0,0 }, _float2 vOffSetPos = { 0.f,0.f }, _float2 vOffSetScale = { 1.f,1.f }, _float fLifeTime = 0.1f);

	void Set_CurMapType(MAP_TYPE eType) { m_eCurMapType = eType; }
private: /* For.Distortion Variable */
	vector<DISTORTION_DESC>		m_Distortions;
	
	class CTransform*			m_pDistortionTransformCom = { nullptr };
	class CTexture*				m_pDistortionTextureCom = { nullptr };
	class CShader*				m_pDistortionShaderCom = { nullptr };
	class CTexture*				m_pAuraTextureCom = { nullptr };

	ID3D11ShaderResourceView*	m_pBackBufferSRV = { nullptr };

	_float m_fAuraAccTime = { 0.f };
	_float m_fAccTime = { 0.f };
	_uint m_iRenderPlayerCount = { 0 };
	
	_bool m_isFriezaRender = { false };
	MAP_TYPE m_eCurMapType = { MAP_SPACE };
public:
	void Set_AuraColor(_float4 vColor) { m_fAuraColor = vColor; }
private:
	_float4 m_fAuraColor = {};
private:
	HRESULT Render_Priority(_float fTimeDelta);
	HRESULT Render_ShadowObj(_float fTimeDelta);
	HRESULT Render_Glow_Priority(_float fTimeDelta);
	HRESULT Render_Blend_Priority(_float fTimeDelta);
	HRESULT Render_StageDepth(_float fTimeDelta);
	HRESULT Render_StageDeferred(_float fTimeDelta);
	HRESULT Render_Map(_float fTimeDelta);
	HRESULT Render_NonBlend(_float fTimeDelta);
	HRESULT Render_Player(_float fTimeDelta);
	HRESULT Render_PlayerLight(_float fTimeDelta, _int iCount);
	HRESULT Render_PlayerDeferred(_float fTimeDelta);
	HRESULT Render_PlayerAuraMaskBlur(_float fTimeDelta, _float4 vColor);
	HRESULT Render_PlayerBlur(_float fTimeDelta);
	HRESULT Render_NonBlend_Test(_float fTimeDelta); // 디버깅용 피킹
	HRESULT Render_NonBlend_Layer(_float fTimeDelta);// 디버깅용 피킹
	HRESULT Render_Lights(_float fTimeDelta);
	HRESULT Render_Deferred(_float fTimeDelta);
	HRESULT Render_NonLight(_float fTimeDelta);
	HRESULT Render_NonLight_Effect(_float fTimeDelta);
	HRESULT Render_Glow(_float fTimeDelta);
	HRESULT Render_AllGlow_Effect_BackSide(_float fTimeDelta);
	HRESULT Render_AllGlow_Effect_Pri(_float fTimeDelta);
	HRESULT Render_Blend(_float fTimeDelta);
	HRESULT Render_UI(_float fTimeDelta);
	HRESULT Render_Glow_UI(_float fTimeDelta);
	HRESULT Render_MultyGlow_UI(_float fTimeDelta);
	HRESULT Render_AllGlow_Effect(_float fTimeDelta);
	HRESULT Render_CutScene_Pri_Effect(_float fTimeDelta);
	HRESULT Render_CutScene_Object(_float fTimeDelta);
	HRESULT Render_CutScene_Late_Effect(_float fTimeDelta);
	HRESULT Render_Node(_float fTimeDelta);


	HRESULT Initialize_RenderTarget();
private:
	HRESULT Render_Debug(_float fTimeDelta);

	HRESULT Render_Distortion(_float fTimeDelta);
	HRESULT Render_Metallic(_float fTimeDelta);

	HRESULT Draw_AllBlackOut(_float fTimeDelta);
	HRESULT Draw_AllWhiteOut(_float fTimeDelta);

	HRESULT Draw_MapBlackOut(_float fTimeDelta);
	HRESULT Draw_OutLine_Effect();
	HRESULT Draw_AllGlow_Effect(_int isPri);
	HRESULT Draw_Glow(CShader* pShader , GLOW_DESC* pDesc = nullptr);
	HRESULT Draw_MapBloom();
	HRESULT Draw_WhiteBlack_Mode(_float fTimeDelta);
	HRESULT Draw_Test_PostProcess(_float fTImeDelta);

public:
	void Switch_BlackOut(_bool isTrue);
	void Switch_AllBlackOut();
	void Start_AllWhiteOut(_float fWhiteTime,_float fSpeed);
	void Start_WhiteOut(_float2 vDir, _bool* isDone, _float fWhiteSpeed);
	void Switch_Test_PostProcess(_bool isTrue);
public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END