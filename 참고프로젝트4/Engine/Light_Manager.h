#pragma once

#include "Base.h"

BEGIN(Engine)
class CGameObject;

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	HRESULT Initialize();
	
	HRESULT Add_Light(const LIGHT_DESC& LightDesc, shared_ptr<class CLight>* ppOut = nullptr );
	HRESULT Add_Light(string FilePath, vector<shared_ptr<class CLight>>* LightVector = nullptr);

	HRESULT Add_ShadowLight(const SHADOWLIGHT_DESC& ShadowLightDesc);
	void	Release_Light(shared_ptr<class CLight> pLight);
	void	Reset_Light();
	void	Tick(_cref_time fTimeDelta);
	HRESULT Render(shared_ptr<class CShader> pShader, shared_ptr<class CVIBuffer_Rect> pVIBuffer);

	shared_ptr<class CLight>	Make_Light_On_Owner(weak_ptr<CGameObject> _pOwner, _float4 vColor, _float fRange,
		_float3 vPosOffset, _bool bUseVolumetric, _float4 vAmbient);

	void	Set_CullLength(_float fLength) { m_fCullLength = fLength; }
	void	Set_Culling(_bool bUseCull) { m_bUseCulling = bUseCull; }

private:
	_bool									m_bUseCulling = { true };
	list<shared_ptr<class CLight>>			m_Lights;
	weak_ptr<class CLight>					m_pDirectionalLight;
	_float4x4								m_ShadowLight_ViewMatrix = {};
	_float4x4								m_ShadowLight_ProjMatrix = {};
	_float									m_fCullLength = { 50.f };	// 컬링 거리조절

public:
	_float4x4 Get_ShadowLight_ViewMatrix() { return m_ShadowLight_ViewMatrix; }
	_float4x4 Get_ShadowLight_ProjMatrix() { return m_ShadowLight_ProjMatrix; }

	weak_ptr<class CLight> Get_DirectionalLight() { return m_pDirectionalLight; }
	shared_ptr<class CLight> Find_Light(string LightName);

	void Clear() { Free(); };

public:
	static CLight_Manager* Create();
	virtual void Free() override;
};

END