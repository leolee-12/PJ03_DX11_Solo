#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CTransform;
class CGameInstance;
class CCollider;
END

BEGIN(Client)

class CEffect_Layer : public CBase
{
public:
	typedef struct
	{
		_float3 vPosition;
		_float3 vScaled;
		_float3 vRotation;

		const _float4x4* pPlayertMatrix;

	}LAYER_DESC;
	typedef struct
	{
		const _float4x4* pPlayertMatrix = { nullptr };
		CTransform* pTransformCom = { nullptr };
		_int m_isPlayerDirRight = { 1 };
	}COPY_DESC;


private:
	CEffect_Layer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Layer(const CEffect_Layer& Prototype);
	virtual ~CEffect_Layer() = default;

public:
	HRESULT Initialize_Prototype(void* pArg);

	HRESULT Initialize(COPY_DESC* pArg = nullptr, _bool isBillboading = false);
	void Camera_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	HRESULT	Render(_float fTimeDelta);

public:
	HRESULT Add_Effect(class CEffect* pEffect);
	vector<class CEffect*> Get_Effects();
	class CEffect* Find_Effect(const std::wstring& effectName);
	HRESULT Play_Effect_Animation(_float fTimeDelta);
	HRESULT Set_In_Layer_Effect();
	void Set_Animation_Position(_float fNewCurPos);

	HRESULT Set_Layer_Scaled(_float3 ChangeScaled);
	HRESULT Set_Layer_Position(_float3 ChangePosition);
	HRESULT Set_Layer_Rotation(_float3 ChangeRotation);

	HRESULT Set_Copy_Layer_Scaled(_float3 ChangeScaled);
	HRESULT Set_Copy_Layer_Position(_float3 ChangePosition);
	HRESULT Set_Copy_Layer_Rotation(_float3 ChangeRotation);

	_float3 Get_Layer_Scaled();
	_float3 Get_Layer_Position();
	_float3 Get_Layer_Rotation();


public:
	class CTransform* m_pTransformCom = { nullptr };
	class CTransform* m_pCopyTransformCom = { nullptr };
	class CTransform* m_pRotationTransformCom = { nullptr };
	vector<class CEffect*>			m_MixtureEffects;

	_uint			m_iNumKeyFrames = { 0 };
	_float			m_fDuration = { 0.f };
	_float			m_fTickPerSecond = {0.f};
	_float			m_fCurrentAnimPosition = { 0.f };
	_bool			m_bIsRender = { true };
	_bool						m_bIsDoneAnim = { false };
	_bool						m_bIsCopy = { false };
	_bool						m_bIsFollowing = { false };

private:
	_uint							m_iNumEffects = {0};
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };
	CCollider*					m_pColliderCom = { nullptr };
	const _float4x4*			 m_pPlayerMatrix = { nullptr };
	const CTransform*			m_pPlayerTransformCom = { nullptr };
	_matrix						LayerMatrix;
	_float3							m_fChangePosition = { 0.f, 0.f, 0.f };

	//_bool m_isChase
	
public:
	static CEffect_Layer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	CEffect_Layer* Clone(COPY_DESC* pArg = nullptr, _bool isBillboading = false);
	virtual void Free() override;
};

END