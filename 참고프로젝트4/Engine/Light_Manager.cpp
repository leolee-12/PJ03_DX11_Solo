#include "Light_Manager.h"
#include "Light.h"

#include "GameInstance.h"

CLight_Manager::CLight_Manager()
{
}

HRESULT CLight_Manager::Initialize()
{

	return S_OK;
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc, shared_ptr<class CLight>* ppOut)
{
	shared_ptr<CLight> pLight = CLight::Create(LightDesc);
	if (nullptr == pLight)
		RETURN_EFAIL;

	shared_ptr<CLight> pFindLight = Find_Light(pLight->Get_LightDesc().strLightName);

	if (pFindLight != nullptr)
	{
		pFindLight->Set_LightDesc(LightDesc);
		Release_Light(pLight);
	}
	else {
		m_Lights.push_back(pLight);

		if (LightDesc.eType == LIGHT_DESC::TYPE_DIRECTIONAL)
		{
			m_pDirectionalLight = pLight;
			SHADOWLIGHT_DESC Desc;

			//XMStoreFloat4(&Desc.vEye ,XMLoadFloat4(&LightDesc.vDirection )* -20.f);
			//Desc.vEye.w = 1.f;
			Desc.vEye = _float4(-20.f, 20.f, -20.f, 1.f);
			Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
			Add_ShadowLight(Desc);
		}

	}
		if (ppOut != nullptr)
			*ppOut = pLight;
	return S_OK;
}

HRESULT CLight_Manager::Add_Light(string FilePath, vector<shared_ptr<class CLight>>* LightVector)
{
	json In_Json;
	if (FAILED(CJson_Utility::Load_Json(FilePath.c_str(), In_Json)))
		return E_FAIL;

	if (In_Json.contains("LightData"))
	{
		int index = 0;
		for (auto& OBJ : In_Json["LightData"])
		{
			LIGHT_DESC pDesc;
			shared_ptr<CLight> pLight = nullptr;

			pDesc.eType = In_Json["LightData"][index]["Type"];
			pDesc.fRange = In_Json["LightData"][index]["fRange"];
			pDesc.fSpotPower = In_Json["LightData"][index]["fSpotPower"];
			pDesc.strLightName = In_Json["LightData"][index]["strLightName"];
			pDesc.bUseVolumetric = In_Json["LightData"][index]["bUseVolumetric"];

			pDesc.fThetaPi = _float2(In_Json["LightData"][index]["fThetaPi"][0], In_Json["LightData"][index]["fThetaPi"][1]);
			pDesc.vAmbient = _float4(In_Json["LightData"][index]["vAmbient"][0], In_Json["LightData"][index]["vAmbient"][1], In_Json["LightData"][index]["vAmbient"][2], In_Json["LightData"][index]["vAmbient"][3]);
			pDesc.vDiffuse = _float4(In_Json["LightData"][index]["vDiffuse"][0], In_Json["LightData"][index]["vDiffuse"][1], In_Json["LightData"][index]["vDiffuse"][2], In_Json["LightData"][index]["vDiffuse"][3]);
			pDesc.vDirection = _float4(In_Json["LightData"][index]["vDirection"][0], In_Json["LightData"][index]["vDirection"][1], In_Json["LightData"][index]["vDirection"][2], In_Json["LightData"][index]["vDirection"][3]);
			pDesc.vEmissive = _float4(In_Json["LightData"][index]["vEmissive"][0], In_Json["LightData"][index]["vEmissive"][1], In_Json["LightData"][index]["vEmissive"][2], In_Json["LightData"][index]["vEmissive"][3]);
			pDesc.vPosition = _float4(In_Json["LightData"][index]["vPosition"][0], In_Json["LightData"][index]["vPosition"][1], In_Json["LightData"][index]["vPosition"][2], In_Json["LightData"][index]["vPosition"][3]);
			pDesc.vSpecular = _float4(In_Json["LightData"][index]["vSpecular"][0], In_Json["LightData"][index]["vSpecular"][1], In_Json["LightData"][index]["vSpecular"][2], In_Json["LightData"][index]["vSpecular"][3]);

			Add_Light(pDesc, &pLight);

			if (LightVector != nullptr)
				LightVector->push_back(pLight);

			if (pDesc.eType == LIGHT_DESC::TYPE_DIRECTIONAL)
			{
				m_pDirectionalLight = pLight;
				SHADOWLIGHT_DESC Desc;

				XMStoreFloat4(&Desc.vEye, XMLoadFloat4(&pDesc.vDirection) * -50.f);
				Desc.vEye.w = 1.f;
				Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
				Add_ShadowLight(Desc);
			}
			index++;
		}//로드 끝
	}

	return S_OK;
}

HRESULT CLight_Manager::Add_ShadowLight(const SHADOWLIGHT_DESC& ShadowLightDesc)
{
	XMStoreFloat4x4(&m_ShadowLight_ViewMatrix, XMMatrixLookAtLH(XMLoadFloat4(&ShadowLightDesc.vEye), XMLoadFloat4(&ShadowLightDesc.vAt), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&m_ShadowLight_ProjMatrix, XMMatrixPerspectiveFovLH(ShadowLightDesc.fFovy, ShadowLightDesc.fAspect, ShadowLightDesc.fNear, ShadowLightDesc.fFar));

	//XMStoreFloat4x4(&m_ShadowLight_ViewMatrix, XMMatrixLookAtLH(XMVectorSet(-20.f, 20.f, -20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	//XMStoreFloat4x4(&m_ShadowLight_ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), 1280.f / (float)720.f, 0.1f, 300.f));

	return S_OK;
}

void CLight_Manager::Release_Light(shared_ptr<CLight> pLight)
{
	// pLight가 m_Lights에 존재하는지 확인
	auto iter = find(m_Lights.begin(), m_Lights.end(), pLight);
	if (iter == m_Lights.end()) {
		// pLight가 m_Lights에 존재하지 않음

		pLight.reset();
		return;
	}

	// m_Lights에서 pLight를 제거
	(*iter).reset();
	m_Lights.erase(iter);
}

void CLight_Manager::Reset_Light()
{
	if (!m_Lights.empty()) {
		for (auto& pLight : m_Lights)
			pLight.reset();
		m_Lights.clear();
	}

	if (m_pDirectionalLight.lock() != nullptr) {
		m_pDirectionalLight.lock().reset();
		m_pDirectionalLight.lock() = nullptr;
	}
}

void CLight_Manager::Tick(_cref_time fTimeDelta)
{
	//for each 문은 중간에 삭제가 일어나면 터짐

	for (auto iter = m_Lights.begin(); iter != m_Lights.end(); )
	{
		if ((*iter)->Get_Dead() || ((*iter)->Is_UseLifeTime() && (*iter)->Tick_LifeTime(fTimeDelta)))
		{			
			(*iter).reset();
			iter = m_Lights.erase(iter);
		}
		else {
			(*iter)->Tick(fTimeDelta);
			iter++;
		}
	}
}

HRESULT CLight_Manager::Render(shared_ptr<CShader> pShader, shared_ptr<CVIBuffer_Rect> pVIBuffer)
{
	/*FRapidJson::Open_Json(GI()->MainDataPath() + L"JSON_Script/Light.json")
		.Read_Data("Light", m_fCullLength);*/

	for (auto& pLight : m_Lights)
	{
		if (pLight->Get_TurnOnLight())
		{
			_vector vPos = GI()->Get_CamPosition();
			_float fLength = XMVector3Length(vPos - pLight->Get_LightPosition()).m128_f32[0];
			_float fRadius = pLight->Get_LightDesc().fRange;
			_bool bIsInLength = (fLength < m_fCullLength + fRadius);
			_bool bIsInLength_Const = (fLength < 10.f + fRadius);
			
			// 거리 안에 들거나 거리가 멀면서 절두체 안에 들 경우 렌더링
			if (!m_bUseCulling || bIsInLength_Const || bIsInLength && pLight->Intersects(*GI()->Get_WorldFrustumPtr()))
			{
				pLight->Render(pShader, pVIBuffer);
			}
		}
	}

	return S_OK;
}

shared_ptr<class CLight> CLight_Manager::Make_Light_On_Owner(weak_ptr<CGameObject> _pOwner, _float4 vColor, _float fRange,
	_float3 vPosOffset, _bool bUseVolumetric, _float4 vAmbient)
{	
	shared_ptr<CLight> pLight = nullptr;

	if (_pOwner.lock() != nullptr)
	{
		
		LIGHT_DESC			LightDesc{};

		LightDesc.eType = LIGHT_DESC::TYPE_POINT;
		string ModelPath = WstrToStr(_pOwner.lock()->Get_ModelCom().lock()->Get_ModelPath());
		LightDesc.strLightName = ModelPath + to_string(m_Lights.size()) + "Light";
		_float3 fPos = _pOwner.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
		fPos += vPosOffset; // 오프셋 추가
		LightDesc.vPosition = _float4(fPos.x, fPos.y, fPos.z, 1.f);
		LightDesc.fRange = fRange;
		LightDesc.vDiffuse = vColor;
		LightDesc.vAmbient = vAmbient;
		LightDesc.vSpecular = _float4(0.1f, 0.1f, 0.1f, 1.f);
		LightDesc.vEmissive = _float4(0.f, 0.f, 0.f, 1.f);
		LightDesc.bUseVolumetric = bUseVolumetric;
		LightDesc.fSpotPower = 1.f;

		Add_Light(LightDesc, &pLight);

		if (pLight != nullptr)
		{
			pLight->Set_Owner(_pOwner);
		}
	} // 성공하면 빛을 반환 실패하면 null 반환

	return pLight;
}

shared_ptr<class CLight> CLight_Manager::Find_Light(string LightName)
{
	if (LightName == "")
		return nullptr;

	for (auto& pLight : m_Lights)
	{
		if (pLight->Get_LightDesc().strLightName == LightName)
		{
			return pLight;
		}
	}

	return nullptr;
}

CLight_Manager* CLight_Manager::Create()
{
	CLight_Manager* pInstance = new CLight_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLight_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		pLight.reset();

	m_Lights.clear();
}
