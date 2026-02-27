#include "stdafx.h"
#include "Line_Draw.h"
#include "RenderInstance.h"
#include "GameInstance.h"
#include "Character.h"
CLine_Draw::CLine_Draw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CLine_Draw::CLine_Draw(const CLine_Draw& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CLine_Draw::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLine_Draw::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CLine_Draw::Camera_Update(_float fTimeDelta)
{
}

void CLine_Draw::Update(_float fTimeDelta)
{
}

void CLine_Draw::Late_Update(_float fTimeDelta)
{

}

HRESULT CLine_Draw::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

void CLine_Draw::TextRender(const vector<CameraPoint>& points, _float fTimeDelta)
{
    // 카메라의 뷰 매트릭스와 프로젝션 매트릭스 가져오기
    _float4x4 viewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
    _float4x4 projMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

    // 뷰와 프로젝션 매트릭스를 _matrix로 로드
    _matrix view = XMLoadFloat4x4(&viewMatrix);
    _matrix proj = XMLoadFloat4x4(&projMatrix);
    _matrix viewProj = XMMatrixMultiply(view, proj);

    _vector cameraPosition = m_pGameInstance->Get_CamPosition_Vector();

    // 윈도우(스크린) 크기 가져오기
    int screenWidth = g_iWinSizeX;
    int screenHeight = g_iWinSizeY;

    // **플레이어의 방향 가져오기 (1이면 그대로, -1이면 반전)**
    CCharacter* character = static_cast<CCharacter*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Character"), 0));
    _int direction = character->Get_iDirection();

    for (size_t i = 0; i < points.size(); ++i)
    {
        const CameraPoint& point = points[i];

        // 로컬 포지션 로드
        _vector localPosVec = XMLoadFloat3(&point.position);

        // **direction에 따른 포지션 조정**
        if (direction == -1)
        {
            // x축 부호 반전
            localPosVec = XMVectorSetX(localPosVec, -XMVectorGetX(localPosVec));
        }

        // 월드 매트릭스 로드
        if (point.hasWorldFloat4x4 && point.pWorldFloat4x4)
        {
            _float4x4 worldMatrix = *point.pWorldFloat4x4;
            _matrix matWorld = XMLoadFloat4x4(&worldMatrix);

            // **스케일링 제거를 위한 행렬 분해**
            _vector modelScale;
            _vector modelRotationQuat;
            _vector modelTranslation;
            XMMatrixDecompose(&modelScale, &modelRotationQuat, &modelTranslation, matWorld);

            // **스케일링이 제거된 월드 행렬 재구성**
            _matrix rotationMatrix = XMMatrixRotationQuaternion(modelRotationQuat);
            _matrix translationMatrix = XMMatrixTranslationFromVector(modelTranslation);
            _matrix matWorldNoScale = rotationMatrix * translationMatrix;

            // 로컬 포지션을 스케일링이 제거된 월드 행렬로 변환
            _vector posWorld = XMVector3TransformCoord(localPosVec, matWorldNoScale);

            // 카메라와의 거리를 계산
            _vector distanceVec = posWorld - cameraPosition;
            float distance = XMVectorGetX(XMVector3Length(distanceVec));

            // 거리에 따라 폰트 크기를 조정
            float baseFontSize = 1.0f;
            float adjustedFontSize = baseFontSize / (distance * 0.1f);

            // 월드 좌표를 클립 공간으로 변환
            _vector posClip = XMVector3TransformCoord(posWorld, viewProj);

            // 클립 공간에서 NDC로 변환
            _float3 posNDC;
            XMStoreFloat3(&posNDC, posClip);

            // NDC를 윈도우 좌표로 변환
            float x = (posNDC.x + 1.0f) * 0.5f * static_cast<float>(screenWidth);
            float y = (1.0f - posNDC.y) * 0.5f * static_cast<float>(screenHeight);

            // 텍스트 위치 설정
            _float2 textPos(x, y);

            // 인덱스 값을 문자열로 변환
            TCHAR szCount[10];
            swprintf_s(szCount, TEXT("%zu"), i);

            // 텍스트 색상 설정 (빨간색)
            _vector textColor = XMVectorSet(1.f, 0.f, 0.f, 1.f);

            // 텍스트 렌더링
            m_pGameInstance->Draw_Font(
                TEXT("Font_Nexon"),
                szCount,
                textPos,
                textColor,
                0.f,
                { 0.f, 0.f },
                adjustedFontSize * 1.5f
            );
        }
    }
}

void CLine_Draw::Set_LinePoints(const vector<CameraPoint>& points, const _float3& vStart, const _float3& vEnd)
{
	m_vStartPoint = vStart;
	m_vEndPoint = vEnd;

	_float4 CamPosition = m_pGameInstance->Get_CamPosition_Float4();
	_float3 CamPositionFloat3 = _float3(CamPosition.x, CamPosition.y, CamPosition.z);

	m_pVIBufferCom->CalculateQuad(vStart, vEnd, m_fThickness, CamPositionFloat3);
}

HRESULT CLine_Draw::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Line_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLine_Draw::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float4 color = { 0.f, 1.f, 0.f, 1.f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &color, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CLine_Draw* CLine_Draw::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) {
	CLine_Draw* pInstance = new CLine_Draw(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype())) {
		MessageBox(0, L"CLine_Draw Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CLine_Draw::Clone(void* pArg) {
	CLine_Draw* pClone = new CLine_Draw(*this);
	if (FAILED(pClone->Initialize(pArg))) {
		MessageBox(0, L"CLine_Draw Cloned Failed", L"System Error", MB_OK);
		Safe_Release(pClone);
	}
	return pClone;
}

void CLine_Draw::Free() {

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	__super::Free();
}