#include "stdafx.h"
#include "Particle.h"

#include "GameInstance.h"

IMPLEMENT_CREATE(CParticle)
IMPLEMENT_CREATE_EX1(CParticle, string, strFilePath)
IMPLEMENT_CLONE(CParticle, CGameObject)

CParticle::CParticle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CEffect(pDevice, pContext)
{

}

CParticle::CParticle(const CParticle& rhs)
	: CEffect(rhs)
	/*, m_eParticleType(rhs.m_eParticleType)
	, m_strTextureTag(rhs.m_strTextureTag)*/
	, m_pInstancingDesc(rhs.m_pInstancingDesc)
	, m_tParticle_Desc(rhs.m_tParticle_Desc)
{
}

HRESULT CParticle::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CParticle::Initialize_Prototype(string strFilePath)
{
	json In_Json;
	CJson_Utility::Load_Json(strFilePath.c_str(), In_Json);

	shared_ptr<CVIBuffer_Particle_Point> pVIBufferCom = CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext);
	m_Components.emplace(TEXT("Com_VIBuffer"),pVIBufferCom);

	Load_Json(In_Json["GameObject"]);

	m_pInstancingDesc = pVIBufferCom->Get_InstancingDesc();

	m_bLoad = true;

	// 데이터를 저장.

	return S_OK;
}

HRESULT CParticle::Initialize(void* pArg)
{
	if (!m_bLoad)
	{
		m_tParticle_Desc = *(PARTICLE_DESC*)pArg;
		// 로드로 불러온 객체는 데이터들을 가지고 있다.
		// 즉 이는 툴용
	}

	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components()))
		RETURN_EFAIL;

	return S_OK;
}

void CParticle::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);
}

void CParticle::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_fTimeDelta += fTimeDelta;

	if (m_bPlay)
		m_pVIBufferCom->Update(fTimeDelta);

	UpdateWorldMatrix(m_tParticle_Desc); // 월드행렬 업데이트 계산식.
}
 
void CParticle::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CParticle::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Judge_Dead(m_tParticle_Desc); // 죽음 조건 판단
}

void CParticle::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (m_bParticleDead)
		return;
	//RENDER_BLEND_PARTICLE -> 이걸로 하면 메쉬가 파티클을 잡아먹음. 시간이 없으니 렌더 그룹을 바꿔주는 것으로 대신.
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND_EFFECT, shared_from_this())))
		return;
}

HRESULT CParticle::Render()
{
	if (FAILED(Bind_ShaderResources()))
		RETURN_EFAIL;

	m_pShaderCom->Begin(m_tParticle_Desc.iShaderPassType);

	m_pVIBufferCom->Bind_VIBuffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

void CParticle::Reset_Effect(_bool bActivate)
{
	/*if (!bGroup && !IsState(OBJSTATE::Active))
		TurnOn_State(OBJSTATE::Active);*/
	// 이렇게 해논 이유가 툴에서 사용하려고 한 것.
	// 차라리 툴에서 사용하는 것과 게임 내에서 사용하는 것으로 나눠야 한다.
	// 구분을 제대로 해주자.

	if(bActivate && !m_bIsInGroupEffect && m_pGameInstance->Get_CreateLevelIndex()==LEVEL_TOOL)
		TurnOn_State(OBJSTATE::Active); // 툴에서 비활성화 시킨 이펙트를 다시 활성화 시킴.
	// bActivate 이면 실행

	// 게임에서 그룹과 단일은 각자 비활성화 및 활성화를 담당하는 대체자가 있음.
	
	//Set_Owner(nullptr); // 오너 리셋

	Reset_Frame(); // 스프라이트를 위함
	Reset_AllParticles(); // 파티클 인스턴싱 정보들 초기 값으로 리셋.

	m_bParticleDead = false; // 죽음 처리x
}

void CParticle::Reset_Prototype_Data()
{
	json In_Json;
	string strFilePath = WstrToStr(CUtility_File::Get_FilePath(CPath_Mgr::FILE_TYPE::DATA_FILE, Get_PrototypeTag()));

	if (Compare_Str(strFilePath, "Not_Find"))
		return;

	CJson_Utility::Load_Json(strFilePath.c_str(), In_Json);

	if (m_pVIBufferCom)
	{
		Load_Json(In_Json["GameObject"]);

		m_pInstancingDesc = dynamic_pointer_cast<CVIBuffer_Instancing>(m_pVIBufferCom)->Get_InstancingDesc();
		// 데이터를 다시 받아옴.
	}
	else {
		/*shared_ptr<CVIBuffer_Particle_Point> pVIBufferCom = CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext);
		m_Components.emplace(TEXT("Com_VIBuffer"), pVIBufferCom);*/

		Load_Json(In_Json["GameObject"]);

		auto ParticleInstancing = m_Components.find(TEXT("Com_VIBuffer"))->second;
		if (ParticleInstancing == nullptr)
			return;

		m_pInstancingDesc = dynamic_pointer_cast<CVIBuffer_Particle_Point>(ParticleInstancing)->Get_InstancingDesc();//pVIBufferCom->Get_InstancingDesc();
	}
	
	m_bLoad = true;

	// 모든 데이터를 다시 받아오고 나서 기존에 남아있는 클론들은 삭제 예정.

	//Reset_Effect();
}

HRESULT CParticle::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_Shader_Particle_Point"),
		TEXT("Com_Shader"), & (m_pShaderCom))))
		RETURN_EFAIL;
	/* For.Com_VIBuffer */
	CVIBuffer_Instancing::INSTANCING_DESC		ParticleDesc = {};
 	if (m_bLoad)
		ParticleDesc = *m_pInstancingDesc;
	else
	{
		ParticleDesc = m_tParticle_Desc.Instancing_Desc;
		ParticleDesc.iNumInstance = m_tParticle_Desc.iNumInstnace;
		ParticleDesc.iMaskArrayMaxIndex = m_tParticle_Desc.iMaskArrayNum;
	}
		
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_VIBuffer_Particle_Point"),
		TEXT("Com_VIBuffer"), & (m_pVIBufferCom), &ParticleDesc)))
		RETURN_EFAIL;

	/* For.Com_Material */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), &(m_pMaterialCom))))
		RETURN_EFAIL;

	m_pMaterialCom->Ready_CustomSingleMaterial(MATERIALTYPE::MATERIATYPE_DIFFUSE, m_tParticle_Desc.strDiffuse, 1);
	//m_pMaterialCom->Ready_CustomSingleMaterial(MATERIALTYPE::MATERIATYPE_MASK, m_tParticle_Desc.strMask, 1);
	m_pMaterialCom->Ready_CustomSingleMaterial(MATERIALTYPE::MATERIATYPE_NOISE, m_tParticle_Desc.strNoise, 1);
	
	/* For.Com_MaskMaterial */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_Material"),
		TEXT("Com_MaskMaterial"), &(m_pMaskArrayCom))))
		RETURN_EFAIL;

	for (_uint i = 0; i < 18; i++)
	{
		m_pMaskArrayCom->Ready_CustomSingleMaterial(i, m_tParticle_Desc.strMaskArray[i], 1);
	}

	return S_OK;
}

HRESULT CParticle::Bind_ShaderResources()
{	
	// For.Matrix
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_matWorld)))
		RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
		RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
		RETURN_EFAIL;

	// For.TexTure
	if (FAILED(m_pMaterialCom->Bind_SRVToShader(m_pShaderCom.get(), "g_DiffuseTexture", MATERIALTYPE::MATERIATYPE_DIFFUSE, 0,true))) RETURN_EFAIL;
	if (FAILED(m_pMaterialCom->Bind_SRVToShader(m_pShaderCom.get(), "g_NoiseTexture", MATERIALTYPE::MATERIATYPE_NOISE, 0, true))) RETURN_EFAIL;
	if (FAILED(m_pMaskArrayCom->Bind_SRVToShaders(m_pShaderCom.get(), "g_MaskTextures", m_tParticle_Desc.iMaskArrayNum))) RETURN_EFAIL;
	if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShaderCom.get(), "g_DepthTexture")))
		RETURN_EFAIL;

	// For.UV
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDiffuseUV", &m_tParticle_Desc.vDiffuseUV, sizeof(_float2)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskUV", &m_tParticle_Desc.vMaskUV, sizeof(_float2)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vNoiseUV", &m_tParticle_Desc.vNoiseUV, sizeof(_float2)))) RETURN_EFAIL;

	// For.Distirtion
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDistortionValue", &m_tParticle_Desc.vDistortionValue, sizeof(_float3)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDistortionScale", &m_tParticle_Desc.fDistortionScale, sizeof(_float)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDistortionBias", &m_tParticle_Desc.fDistortionBias, sizeof(_float)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDistortionWeight", &m_tParticle_Desc.fNoiseWeight, sizeof(_float)))) RETURN_EFAIL;

	// For.Sprite
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bSprite", &m_tParticle_Desc.bSpriteAnim, sizeof(_bool)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iSpriteType", &m_tParticle_Desc.iSpriteType, sizeof(_int)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vSpriteColRow", &m_tParticle_Desc.vSpriteColRow, sizeof(_float2)))) RETURN_EFAIL;

	// Etc
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_tParticle_Desc.fDiscard_Alpha, sizeof(_float)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeDelta", &m_fTimeDelta, sizeof(_float)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_tParticle_Desc.vSolid_Color, sizeof(_float4)))) RETURN_EFAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamLook", &m_pGameInstance->Get_CamLook(), sizeof(_float4))))
		RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bSoft", &m_tParticle_Desc.bParticleSoft, sizeof(_bool)))) RETURN_EFAIL;
	// 소프트 유무
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iColorBlendType", &m_tParticle_Desc.eColorBlendType, sizeof(_int)))) RETURN_EFAIL;
	// 컬러 타입
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bSolidColor", &m_tParticle_Desc.bSolidColor, sizeof(_bool)))) RETURN_EFAIL;
	// 단색 렌더타겟 사용 유무

	return S_OK;
}

void CParticle::Judge_Dead(EFFECT_DESC EffectDesc)
{
	if (m_pVIBufferCom == nullptr)
		return;

	if (EffectDesc.eDeadType != CEffect::DEADTYPE::OWNER)
	{// 죽음 조건이 오너가 아니면
	
		if (!m_bParticleDead && static_pointer_cast<CVIBuffer_Particle_Point>(m_pVIBufferCom)->Is_Finished())
		{

			if (m_bLoad && m_pGameInstance->Get_CreateLevelIndex() != LEVEL_TOOL)
			{ // 게임
				m_bParticleDead = true;
				
				if (m_bIsInGroupEffect)
					TurnOff_State(OBJSTATE::Active); // 그룹 이펙트
				else
					Set_Dead(); // 단일 이펙트 -> 오브젝트 풀로 리턴
			}
			else { // 툴
				m_bParticleDead = true;
				TurnOff_State(OBJSTATE::Active); // 죽음 처리 대신 -> 툴 전용
			}
		}
		
	}
	else {
		// 죽음 조건이 오너이면
		if ((m_pOwner.lock() == nullptr) || m_pOwner.lock()->IsState(OBJSTATE::WillRemoved))
		{
			if (m_bLoad && m_pGameInstance->Get_CreateLevelIndex() != LEVEL_TOOL)
			{ // 게임
				m_bParticleDead = true;

				if(m_bIsInGroupEffect)
					TurnOff_State(OBJSTATE::Active); // 그룹 이펙트
				else
					Set_Dead(); // 단일 이펙트 -> 오브젝트 풀로 리턴
			}
			else {// 툴
				//m_bParticleDead = true;
				//TurnOff_State(OBJSTATE::Active); // 툴 전용
				// 툴에서는 오너를 설정하지 못하기 때문에 계속해서 죽임.
			}
		}
	}
}

void CParticle::Reset_AllParticles()
{
	static_pointer_cast<CVIBuffer_Instancing>(m_pVIBufferCom)->Reset_AllParticles();
}

void CParticle::Write_Json(json& Out_Json)
{

	Out_Json["bMaskArray"] = m_tParticle_Desc.bMaskArray;
	Out_Json["iMaskArrayNum"] = m_tParticle_Desc.iMaskArrayNum;
	for(_uint i =0; i<18;i++)
		Out_Json["strMaskArray"][i] = WstrToStr(m_tParticle_Desc.strMaskArray[i]);

	Out_Json["bParticleSoft"] = m_tParticle_Desc.bParticleSoft;

	Effect_Write_Json(Out_Json,m_tParticle_Desc);
	CGameObject::Write_Json(Out_Json);
}

void CParticle::Load_Json(const json& In_Json)
{
	m_tParticle_Desc.bMaskArray = In_Json["bMaskArray"];
	m_tParticle_Desc.iMaskArrayNum = In_Json["iMaskArrayNum"];
	for (_uint i = 0; i < 18; i++)
		m_tParticle_Desc.strMaskArray[i] = StrToWstr(In_Json["strMaskArray"][i]);

	// ---- Soft ------
	if (In_Json.contains("bParticleSoft"))
	{
		m_tParticle_Desc.bParticleSoft = In_Json["bParticleSoft"];
	}

	Effect_Load_Json(In_Json, &m_tParticle_Desc);
	CGameObject::Load_Json(In_Json); 
}

void CParticle::Free()
{
	__super::Free();

}

