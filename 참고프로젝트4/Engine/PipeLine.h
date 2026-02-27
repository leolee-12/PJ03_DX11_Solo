#pragma once

/* 인게임 내 객체들을 그릴때 공통으로 사용하기위한 뷰, 투영행렬을 보관한다. */
#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
public:
	enum TRANSFORMSTATE { TS_VIEW, TS_PROJ, TS_END };
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix);
	void Set_Transform(TRANSFORMSTATE eState, _float4x4 TransformMatrix);

public:
	_matrix Get_TransformMatrix(TRANSFORMSTATE eState);
	_float4x4 Get_TransformFloat4x4(TRANSFORMSTATE eState);
	_matrix Get_TransformMatrixInverse(TRANSFORMSTATE eState);
	_float4x4 Get_TransformFloat4x4Inverse(TRANSFORMSTATE eState);
	_float4 Get_CamPosition();
	void	Set_CamPosition(_float4 vCamPosition) { m_vCamPosition = vCamPosition; };
	void	Set_CamLook(_float4 vCamLook) { m_vCamLook = vCamLook; };
	_float4	Get_CamLook() { return  m_vCamLook; };
public:
	HRESULT Initialize();
	void Tick();


private:
	_float4x4			m_Transform[TS_END];
	_float4x4			m_Transform_Inverse[TS_END];
	_float4				m_vCamPosition;
	_float4				m_vCamLook;
public:
	static CPipeLine* Create();
	virtual void Free() override;
};

END