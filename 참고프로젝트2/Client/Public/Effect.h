#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Renderer)
class CRenderer;
END
BEGIN(Client)

class CEffect :	public CGameObject
{
public:
	typedef struct
	{
		_bool	bIsCopy = { false };

		_float3 vPosition;
		_float3 vScaled;
		_float3 vRotation;

		_wstring EffectName;

		_wstring ModelName;
		_wstring MaskTextureName;
		_wstring DiffuseTextureName;
		
		EFFECT_TYPE EffectType;

		void* SRV_Ptr = { nullptr };
		_int iRenderIndex;

		_int iUnique_Index = -1;
		_int iPassIndex = 1;
		_int iNumWidthImage = { 1 };
		_int iNumHeightImage = { 1 };

		_float4 vColor;
		_matrix LayerMatrix;

		_bool	bIsBillboarding = { false };

		_float4        vGlowColor;
		_float           fGlowFactor;
		_uint    DerredPassIndex;

		_int isGlow;
	}EFFECT_DESC;


protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect(const CEffect& Prototype);
	virtual ~CEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Priority_Render(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

public:
	void Add_KeyFrame(_uint KeyFrameNumber, EFFECT_KEYFRAME NewKeyFrame);
	void Delete_KeyFrame(_uint KeyFrameNumber);
	_bool	Find_KeyFrame(_uint KeyFrameNumber);
	EFFECT_KEYFRAME Get_KeyFrame(_uint KeyFrameNumber);
	EFFECT_KEYFRAME	Get_Near_Front_KeyFrame(_uint frameNumber);
	void Set_Effect_Scaled(_float3 ChangeScaled);
	void Set_Effect_Position(_float3 ChangePosition);
	void Set_Effect_Rotation(_float3 ChangeRoation);
	void Set_Effect_IsNotPlaying(_bool bIsNotPlaying);
	_float3 Get_Effect_Scaled();
	_float3 Get_Effect_Position();
	_float3 Get_Effect_Rotation();
	HRESULT Play_Animation(_float CurrentFrame);
	void		Set_Effect_Color(_float4 vColor);
	void		Get_Layer_Matrix(_matrix LayerMatrix);
	void		Set_Layer_Matrix();
protected:
	CShader* m_pShaderCom = { nullptr };
	CModel*	 m_pModelCom = { nullptr };
	CTexture* m_pDiffuseTextureCom = { nullptr };
	CTexture* m_pMaskTextureCom = { nullptr };


	_float			m_fCurrentAnimPos = {};
	_float m_fAccTime = { 0.f };

	/* 툴용 쉐이더 패스 인덱스 */

public:
	class CEffect_Animation* m_pAnimation = { nullptr };
	EFFECT_TYPE			m_eEffect_Type = { EFFECT_END };
	_int				m_iUnique_Index = { -1 };
	_wstring		m_EffectName;		// 이거 쉐이더탭으로 가져오기
	_wstring		m_ModelName;
	_wstring		m_MaskTextureName; // 이거 쉐이더탭 저장한 파일 이름
	_wstring		m_DiffuseTextureName;
	_int				m_iNumWidthImage = { 0 };
	_int				m_iNumHeighthImage = { 0 };
	_bool			m_bIsNotPlaying = { false };
	_bool			m_bIsSpriteEnd = { false };
	_bool			m_bIsLoop = { false };
	_float		m_fAlpha = { 0.f };
	_int			m_iRenderIndex = { 0 };
	_int			m_iPassIndex = { 1 };

	_float4		m_vColor = { 0.f, 0.f, 0.f, 30.f };
	_bool			m_IsColorEffect = { false };
	_bool			m_bIsBillboarding = { false };

	_float4        m_vGlowColor = {0.f, 0.f, 0.f, 1.f};
	_float           m_fGlowFactor = { 1.f };
	_uint			m_iDerredPassIndex = {0};

	_bool			m_bIsCopy = { false };
	_int				m_isGlow = { false };
	_bool			m_bIsBackSideEffect = { false };
	_bool			m_bIsShaderLoop = { false };

	EFFECT_DESC			m_ForCopyInform;
	_int	m_iRenderGroupIndex;
	_int m_iChangePassIndex = { 0 };
	_bool m_isInitializeRender = { false };
	_bool m_bIsAlreadyBillboading = { false };

protected:
	virtual HRESULT Ready_Components(_wstring* pModelName, _wstring* pMaskTextureName, _wstring* pDiffuseTexturueName);
	virtual HRESULT Bind_ShaderResources();

	_matrix				m_LayerMatrix = {};
	_float4x4				m_WorldMatrix = {};
public:
	void Set_ParentMatrixMultiply_LocalMatrix();

public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END