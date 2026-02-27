#include "SoundPoint.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

#include "GameInstance.h"
#ifdef _DEBUG
#include "DebugDraw.h"
#endif

CSoundPoint::CSoundPoint()
#ifdef _DEBUG
	: m_bIsCloned(false)
#endif
{

}

CSoundPoint::CSoundPoint(const CSoundPoint& rhs)
	: m_strName(rhs.m_strName), m_SoundPointDesc(rhs.m_SoundPointDesc)
	, m_vRandomRange(rhs.m_vRandomRange)
	, m_bNeedInvalidate_Position(true)
	, m_bNeedInvalidate_Sound(true)
#ifdef _DEBUG
	, m_bIsCloned(true)
	, m_Sphere(rhs.m_Sphere)
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif
{

}

HRESULT CSoundPoint::Initialize(const string& strName, const TSoundPointDesc& SoundPointDesc)
{
	m_strName = strName;
	m_SoundPointDesc = SoundPointDesc;

	m_bNeedInvalidate_Position = true;
	m_bNeedInvalidate_Sound = true;

#if SOUNDPOINT_DEBUG
#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(GI()->Get_D3D11Context().Get());
	m_pEffect = new BasicEffect(GI()->Get_D3D11Device().Get());
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t	iShaderCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderCodeLength);

	if (FAILED(GI()->Get_D3D11Device()
		->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderCodeLength, m_pInputLayout.GetAddressOf())))
		return E_FAIL;
#endif
#endif

	return S_OK;
}

void CSoundPoint::Tick(_cref_time fTimeDelta)
{
	//Update_SoundOwner();
	Invalidate_SoundPoint();
}

HRESULT CSoundPoint::Render()
{
#if SOUNDPOINT_DEBUG
#ifdef _DEBUG
	GI()->Add_DebugEvent(MakeDelegate(this, &CSoundPoint::Render_Debug));

	return S_OK;
#else
	return S_OK;
#endif
#else
	return S_OK;
#endif
}

#ifdef _DEBUG
HRESULT CSoundPoint::Render_Debug()
{
	if (nullptr == m_pBatch || nullptr == m_pEffect || nullptr == m_pInputLayout)
		return E_FAIL;

	m_pBatch->Begin();

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(GI()->Get_TransformMatrix(CPipeLine::TS_VIEW));
	m_pEffect->SetProjection(GI()->Get_TransformMatrix(CPipeLine::TS_PROJ));

	GI()->Get_D3D11Context()->IASetInputLayout(m_pInputLayout.Get());

	m_pEffect->Apply(GI()->Get_D3D11Context().Get());

	DX::Draw(m_pBatch, m_Sphere, Colors::Yellow);

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG


void CSoundPoint::Invalidate_SoundPoint()
{
	_bool bIsPlaying = GI()->IsPlaying_Channel(&m_iPlayingChannel_Index, &m_pPlayingChannel);
	if (!bIsPlaying)
	{
		m_iPlayingChannel_Index = GI()->Play_3DSound(m_SoundPointDesc.GroupNames, m_SoundPointDesc.SoundNames,
			m_SoundPointDesc.eSoundGroup, m_SoundPointDesc.vPosition
			, m_SoundPointDesc.fVolume, m_SoundPointDesc.f3DMin, m_SoundPointDesc.f3DMax
			, m_SoundPointDesc.eRollOffMode
			, &m_pPlayingChannel);
	}

	// 사운드 자체가 변경됨.
	if (m_bNeedInvalidate_Sound)
	{
		m_iPlayingChannel_Index = GI()->Play_3DSound(m_SoundPointDesc.GroupNames, m_SoundPointDesc.SoundNames,
			m_SoundPointDesc.eSoundGroup, m_SoundPointDesc.vPosition
			, m_SoundPointDesc.fVolume, m_SoundPointDesc.f3DMin, m_SoundPointDesc.f3DMax
			, m_SoundPointDesc.eRollOffMode
			, &m_pPlayingChannel);

#ifdef _DEBUG
		m_Sphere.Center = m_SoundPointDesc.vPosition;
		m_Sphere.Radius = 1.f;
#endif // DEBUG

		

		m_bNeedInvalidate_Sound = false;
		m_bNeedInvalidate_Position = false;
	}
	// 위치만 조정됨
	else if (m_bNeedInvalidate_Position)
	{
		GI()->Sound_Move(&m_iPlayingChannel_Index, &m_pPlayingChannel, m_SoundPointDesc.vPosition);

#ifdef _DEBUG
		m_Sphere.Center = m_SoundPointDesc.vPosition;
		m_Sphere.Radius = 1.f;
#endif // DEBUG

		// 초기화 후 변경
		m_bNeedInvalidate_Position = false;
	}
}


void CSoundPoint::Add_SoundData(const string& strGroupName, const string& strSoundName)
{
	m_SoundPointDesc.GroupNames.emplace_back(strGroupName);
	m_SoundPointDesc.SoundNames.emplace_back(strSoundName);

	m_bNeedInvalidate_Sound = true;
}

void CSoundPoint::Set_SoundExtDatas(ESoundGroup eSoundGroup, _float fVolume)
{
	m_SoundPointDesc.eSoundGroup = eSoundGroup;
	m_SoundPointDesc.fVolume = fVolume;

	m_bNeedInvalidate_Sound = true;
}

void CSoundPoint::Clear_SoundNameDatas()
{
	m_SoundPointDesc.GroupNames.clear();
	m_SoundPointDesc.SoundNames.clear();

	m_bNeedInvalidate_Sound = true;
}

void CSoundPoint::Set_Position(_float3 vPos)
{
	m_SoundPointDesc.vPosition = vPos;
	// 빛의 변동을 알리기 위해 설정하는 변수
	m_bNeedInvalidate_Position = true;
}

const _float3 CSoundPoint::Get_Position()
{
	if (m_bNeedInvalidate_Position)
		Invalidate_SoundPoint();

	return m_SoundPointDesc.vPosition;
}


const TSoundPointDesc CSoundPoint::Get_SoundPointDesc()
{
	if (m_bNeedInvalidate_Position)
		Invalidate_SoundPoint();

	return m_SoundPointDesc;
}

void CSoundPoint::Set_SoundPointDesc(const TSoundPointDesc Desc)
{
	m_SoundPointDesc = Desc;

	m_bNeedInvalidate_Position = true;
}

void CSoundPoint::Update_SoundOwner()
{
	/*if (m_bIsGetOwner == true)
	{
		if (m_pOwner.lock() != nullptr)
		{
			_bool IsActiveState = m_pOwner.lock()->IsState(OBJSTATE::Active);
			
			if (true == IsActiveState) { m_bTurnOn = true; }
			else if (false == IsActiveState) { m_bTurnOn = false;  return; }
						
			float3 TargetPos = m_pOwner.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
			float3 LocalOffset = m_pOwner.lock()->Get_PhysXColliderCom().lock()->Get_LocalOffset();
			TargetPos += LocalOffset;

			m_LightDesc.vPosition = { TargetPos.x, TargetPos.y, TargetPos.z, 1.f };
		}
		else if (m_pOwner.lock() == nullptr)			
		{
			Set_Dead();
		}
	}*/
}

void CSoundPoint::Set_Owner(weak_ptr<CGameObject> _pOwner)
{
	if (_pOwner.lock() == nullptr)
		return;
	
	m_pOwner = _pOwner;
}

weak_ptr<CGameObject> CSoundPoint::Get_Owner()
{
	return m_pOwner;
}


shared_ptr<CSoundPoint> CSoundPoint::Create(const string& strName, const TSoundPointDesc& SoundPointDesc)
{
	shared_ptr<CSoundPoint> pInstance(new CSoundPoint(), DELETER(CSoundPoint));

	if (FAILED(pInstance->Initialize(strName, SoundPointDesc)))
	{
		MSG_BOX("Failed to Created : CSoundPoint");
	}
	return pInstance;
}

shared_ptr<CSoundPoint> CSoundPoint::Clone()
{
	shared_ptr<CSoundPoint> pInstance(new CSoundPoint(*this), DELETER(CSoundPoint));

	if (!pInstance)
	{
		MSG_BOX("Failed to Created : CSoundPoint");
	}

	return pInstance;
}


void CSoundPoint::Free()
{
	__super::Free();

	if (GI())
	{
		GI()->Sound_StopByChannel(&m_iPlayingChannel_Index, &m_pPlayingChannel);
	}

#ifdef _DEBUG
	if (!m_bIsCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
#endif
}