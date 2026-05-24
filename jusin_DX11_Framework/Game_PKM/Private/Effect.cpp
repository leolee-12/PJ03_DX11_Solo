#include "Effect.h"
#include "ParticleEmitter.h"
#include "Effect_Mesh.h"
#include "Body.h"
#include "Trail.h" 

#include "GameInstance.h"

CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEffect::CEffect(const CEffect& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_tDesc = *static_cast<EFFECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(&m_tDesc)))
		return E_FAIL;

	/* M8: attach 정보 저장 + identity 보정 */
	m_tAttach = m_tDesc.tAttach;

	/* mLocalOffset이 zero matrix면 identity로 (POD라 _float4x4{} 가 0 행렬) */
	const _float4x4 kZero = {};
	if (0 == memcmp(&m_tAttach.mLocalOffset, &kZero, sizeof(_float4x4)))
		XMStoreFloat4x4(&m_tAttach.mLocalOffset, XMMatrixIdentity());

	Resolve_Attach_Once();

	if (m_tAttach.eKind != EFFECT_DESC::ATTACH_INFO::KIND::NONE
		&& nullptr == m_pAttachMatrix)
	{
#ifdef _DEBUG
		OutputDebugStringA("[Effect] Failed to resolve attach target.\n");
#endif
		return E_FAIL;
	}

	m_pDefinition = m_tDesc.pDefinition;
	if (nullptr == m_pDefinition)
		return E_FAIL;

	/* root transform: vSpawnPos */
	m_pTransformCom->Set_State(STATE::POSITION,
		XMVectorSet(m_tDesc.vSpawnPos.x, m_tDesc.vSpawnPos.y, m_tDesc.vSpawnPos.z, 1.f));

	/* 정의의 각 emitter를 spawn해서 Layer에 등록 (자체 lifecycle 받음) */
	m_Emitters.reserve(m_pDefinition->Emitters.size());

	for (const EMITTER_DEFINITION& emDef : m_pDefinition->Emitters)
	{
		CParticleEmitter::EMITTER_DESC emDesc = Make_EmitterDesc(emDef, _float3(0.f, 0.f, 0.f));
		emDesc.pParentTransform = m_pTransformCom;

		emDesc.iTextureProtoLevel = ETOUI(LEVEL::STATIC);

		CBase* pCloned = m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC),
			PROTO_OBJ_PARTICLE_EMITTER, &emDesc);

		if (nullptr == pCloned)
			return E_FAIL;

		CParticleEmitter* pEmitter = static_cast<CParticleEmitter*>(pCloned);

		if (FAILED(m_pGameInstance->Add_GameObject_Ex(
			m_tDesc.iSpawnLevel, m_tDesc.strLayerTag, pEmitter)))
		{
			Safe_Release(pEmitter);
			return E_FAIL;
		}

		m_Emitters.push_back(pEmitter);
	}

	/* mesh 자식 spawn — Meshes 빈 vector면 자연 skip (기존 emitter-only 정의 회귀 0) */
	m_MeshEmitters.reserve(m_pDefinition->Meshes.size());

	for (const MESH_EFFECT_DEFINITION& mDef : m_pDefinition->Meshes)
	{
		CEffect_Mesh::MESH_EFFECT_DESC mDesc{};
		mDesc.vSpawnPos = _float3(0.f, 0.f, 0.f);
		mDesc.pParentTransform = m_pTransformCom;

		mDesc.strModelProtoTag = mDef.strModelProtoTag;
		mDesc.strShaderProtoTag = (INVALID_TAG == mDef.strShaderProtoTag)
			? PROTO_COM_SHADER_EFFECT_BEAM
			: mDef.strShaderProtoTag;
		mDesc.strTextureProtoTag = mDef.strTextureProtoTag;
		mDesc.iTextureProtoLevel = ETOUI(LEVEL::STATIC);

		mDesc.eBlend = mDef.eBlend;
		mDesc.bIgnoreDepth = mDef.bIgnoreDepth;
		mDesc.eScaleAxis = mDef.eScaleAxis;

		mDesc.curveScale = mDef.curveScale;
		mDesc.curveColor = mDef.curveColor;
		mDesc.curveAlpha = mDef.curveAlpha;
		mDesc.fLifeTime = mDef.fLifeTime;

		mDesc.vStartOffset       = mDef.vStartOffset;
		mDesc.vEmitDirection     = mDef.vEmitDirection;
		mDesc.fEmitConeHalfAngle = mDef.fEmitConeHalfAngle;
		mDesc.vSpeedRange        = mDef.vSpeedRange;
		mDesc.vGravity           = mDef.vGravity;
		mDesc.fSpinSpeedMax      = mDef.fSpinSpeedMax;
		mDesc.fStartDelay        = mDef.fStartDelay;

		const _uint iMeshCount = max(1u, mDef.iCount);
		for (_uint c = 0; c < iMeshCount; ++c)
		{
			CBase* pCloned = m_pGameInstance->Clone_Prototype(
				PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC),
				PROTO_OBJ_EFFECT_MESH, &mDesc);

			if (nullptr == pCloned)
				return E_FAIL;

			CEffect_Mesh* pMesh = static_cast<CEffect_Mesh*>(pCloned);

			if (FAILED(m_pGameInstance->Add_GameObject_Ex(
				m_tDesc.iSpawnLevel, m_tDesc.strLayerTag, pMesh)))
			{
				Safe_Release(pMesh);
				return E_FAIL;
			}

			m_MeshEmitters.push_back(pMesh);
		}
	}

	m_Trails.reserve(m_pDefinition->Trails.size());

	for (const TRAIL_DEFINITION& tDef : m_pDefinition->Trails)
	{
		CTrail::TRAIL_DESC tDesc{};
		tDesc.vSpawnPos = _float3(0.f, 0.f, 0.f);
		tDesc.pParentTransform = m_pTransformCom;
		tDesc.iMaxSegments = tDef.iMaxSegments;
		tDesc.fSegmentSpacing = tDef.fSegmentSpacing;
		tDesc.fLifeTimePerSegment = tDef.fLifeTimePerSegment;
		tDesc.fWidthStart = tDef.fWidthStart;
		tDesc.fWidthEnd = tDef.fWidthEnd;
		tDesc.vUpAxis = tDef.vUpAxis;
		tDesc.eBlend = tDef.eBlend;
		tDesc.bIgnoreDepth = tDef.bIgnoreDepth;
		tDesc.strTextureProtoTag = tDef.strTextureProtoTag;
		tDesc.iTextureProtoLevel = ETOUI(LEVEL::STATIC);
		tDesc.curveColor = tDef.curveColor;

		CBase* pCloned = m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_OBJ_TRAIL, &tDesc);
		if (nullptr == pCloned) return E_FAIL;
		CTrail* pTrail = static_cast<CTrail*>(pCloned);

		if (FAILED(m_pGameInstance->Add_GameObject_Ex(m_tDesc.iSpawnLevel, m_tDesc.strLayerTag,
			pTrail)))
		{
			Safe_Release(pTrail);
			return E_FAIL;
		}
		m_Trails.push_back(pTrail);
	}

	return S_OK;
}

void CEffect::Priority_Update(_float fTimeDelta)
{
}

void CEffect::Update(_float fTimeDelta)
{
	/* M8: Stop 평가는 Late_Update로 이주 (attach 갱신 후 같은 위치에서 처리).
	   Update에서는 아무것도 하지 않는다 (자식 emitter들이 자체 Update 받음). */
	(void)fTimeDelta;
}

void CEffect::Late_Update(_float fTimeDelta)
{
	(void)fTimeDelta;

	if (Is_Dead())
		return;

	/* 1) owner 사망 감지 (BONE/MATRIX 모드) → 즉시 Destroy.
		  bone matrix backing memory 무효화 직전에 deref 회피 (계획서 §함정 F). */
	if (nullptr != m_tAttach.pOwner && m_tAttach.pOwner->Is_Dead())
	{
		Destroy();
		return;
	}

	/* 2) attach matrix deref → effect root world 갱신.
		  mFinal = mLocalOffset × mAttach (× mOwnerWorld for BONE).
		  BONE: bone matrix는 model local space → owner world 곱 필요.
		  MATRIX: pSourceMatrix가 직접 world matrix이면 owner world 곱 불필요. */
	if (nullptr != m_pAttachMatrix)
	{
		_matrix mOffset = XMLoadFloat4x4(&m_tAttach.mLocalOffset);
		_matrix mAttach = XMLoadFloat4x4(m_pAttachMatrix);
		_matrix mFinal = mOffset * mAttach;

		if (m_tAttach.eKind == EFFECT_DESC::ATTACH_INFO::KIND::BONE
			&& nullptr != m_tAttach.pOwner)
		{
			/* owner가 CPartObject 계열(CBody)이면 결합된 world matrix를 써야 한다.
			   m_pTransformCom->Get_WorldMatrixPtr()는 Part의 local matrix(부모 기준)일 뿐. */
			const _float4x4* pOwnerWorld = nullptr;
			if (auto* pBody = dynamic_cast<CBody*>(m_tAttach.pOwner))
				pOwnerWorld = pBody->Get_CombinedWorldMatrixPtr();
			else
				pOwnerWorld = m_tAttach.pOwner->Get_Transform()->Get_WorldMatrixPtr();

			_matrix mOwner = XMLoadFloat4x4(pOwnerWorld);
			mFinal = mFinal * mOwner;
		}

		/* CTransform 전체 setter 없음 — STATE별 4회 (계획서 §함정 §M8 §3) */
		m_pTransformCom->Set_State(STATE::RIGHT, mFinal.r[0]);
		m_pTransformCom->Set_State(STATE::UP, mFinal.r[1]);
		m_pTransformCom->Set_State(STATE::LOOK, mFinal.r[2]);
		m_pTransformCom->Set_State(STATE::POSITION, mFinal.r[3]);
	}

	_bool bAllDead = true;
	for (CParticleEmitter* pEm : m_Emitters)
	{
		if (nullptr != pEm && !pEm->Is_Dead())
		{
			bAllDead = false;
			break;
		}
	}

	if (bAllDead)
	{
		for (CEffect_Mesh* pMesh : m_MeshEmitters)
		{
			if (nullptr != pMesh && !pMesh->Is_Dead())
			{
				bAllDead = false;
				break;
			}
		}
	}

	if (bAllDead)
	{
		for (CTrail* pTrail : m_Trails)
		{
			if (nullptr != pTrail && !pTrail->Is_Dead())
			{
				bAllDead = false;
				break;
			}
		}
	}

	if (bAllDead)
		Set_Dead();
}

HRESULT CEffect::Render()
{
	/* M7a: emitter들이 직접 Render. 본 CEffect는 시각 출력 없음. */
	return S_OK;
}

void CEffect::Stop()
{
	for (CParticleEmitter* pEm : m_Emitters)
	{
		if (nullptr != pEm)
			pEm->Set_Emitting(false);
	}
	/* mesh는 spawn 개념 없음 — lifetime 자연 진행으로 종료 */

	for (CTrail* pTrail : m_Trails)
		if (nullptr != pTrail) pTrail->Stop();
}

void CEffect::Destroy()
{
	for (CParticleEmitter* pEm : m_Emitters)
	{
		if (nullptr != pEm)
		{
			pEm->Clear_All_Particles();
			pEm->Set_Dead();
		}
	}

	for (CEffect_Mesh* pMesh : m_MeshEmitters)
		if (nullptr != pMesh) pMesh->Set_Dead();        // 권장

	for (CTrail* pTrail : m_Trails)
		if (nullptr != pTrail) pTrail->Set_Dead();      // 필수

	Set_Dead();
}

void CEffect::Resolve_Attach_Once()
{
	m_pAttachMatrix = nullptr;

	switch (m_tAttach.eKind)
	{
	case EFFECT_DESC::ATTACH_INFO::KIND::BONE:
	{
		if (nullptr == m_tAttach.pOwner || m_tAttach.strBoneName.empty())
			return;

		if (auto* pBody = dynamic_cast<CBody*>(m_tAttach.pOwner))
		{
			m_pAttachMatrix = pBody->Get_BoneMatrixPtr(m_tAttach.strBoneName.c_str());
			break;
		}

		auto* pModel = static_cast<CModel*>(
			m_tAttach.pOwner->Find_Component(COM_MODEL));
		if (nullptr == pModel)
			return;

		m_pAttachMatrix = pModel->Get_BoneMatrixPtr(m_tAttach.strBoneName.c_str());
		break;
	}
	case EFFECT_DESC::ATTACH_INFO::KIND::MATRIX:
	{
		m_pAttachMatrix = m_tAttach.pSourceMatrix;
		break;
	}
	case EFFECT_DESC::ATTACH_INFO::KIND::NONE:
	default:
		m_pAttachMatrix = nullptr;
		break;
	}
}

CEffect* CEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect* pInstance = new CEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect::Clone(void* pArg)
{
	CEffect* pInstance = new CEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect::Free()
{
	/* borrowed pointers — Layer가 ref 보유. Release 하지 않는다. */
	m_Emitters.clear();
	m_MeshEmitters.clear();
	m_Trails.clear();

	__super::Free();
}