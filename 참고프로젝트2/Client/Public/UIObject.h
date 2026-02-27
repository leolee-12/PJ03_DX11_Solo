#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "GameInstance.h"
#include "UI_Manager.h"

BEGIN(Engine)

class CVIBuffer_Rect;
class CTexture;
class CShader;

END

BEGIN(Client)

class CUIObject abstract : public CGameObject
{
public:
	enum UI_LRPOS  { LEFT , RIGHT  , POS_END };
	enum UI_TYPE { UI_ANIM, UI_NONANIM, TYPE_END };

public:
	typedef struct :public CGameObject::GAMEOBJECT_DESC
	{
		UI_LRPOS eLRPos = {};
		
		UI_TYPE	eType = {};
		_uint iNumUI = {};
		_uint iTotalNum = {};
	}UI_DESC;

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

protected:
	virtual HRESULT Bind_ShaderResources();
	virtual HRESULT Ready_Components();

	//UI 초기 세팅 값
	virtual void Set_UI_Setting(_float fSizeX, _float fSizeY, _float fPosX, _float fPosY, _float fDepth = 0.9f);
	
//UI 크기와 위치를 디버깅해서 찾음
	void DebugTesting(_float fSizeOffset,  _float fDepth = 1.f);

//클릭한 위치가 해당 UI 위치라면 동작
	_bool ClickRange(_float fPickPosX, _float fPickPosY);

public:
	//UI 숨김 / 보여줌
	void IsShow(_bool IsShiw) {m_bIsActive = IsShiw;}

private:
	//매 프레임마다 MainPlayer와 Sub플레이어 데이터 값 갱신 
	void InitPlayer();

private:
	void Add_UIList(CUI_Define::UI_LISTPOS ePos);
	void UI_PosArea(_float fAreaPosY);

protected:
	//넣은 수치값을 직교 X,Y값으로 반환
	_vector GetOffsetPostion(_vector vPosition);

	_bool MoveAnimUI(_vector vTargetPos, _float fSpeed,  _float fDepth , _float fTimeDelta , _float fEndDistance = 15.f);
	_bool Animation(_vector vStartPos ,_vector vTargetPos, _float fSpeed , _float fDepth, _float fTimeDelta,_float fEndDistance = 15.f);

protected:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

protected:
	_bool m_bCharaStun = { FALSE };
	_bool m_bHit = { FALSE };

	UI_LRPOS m_eLRPos = { POS_END };
	UI_DESC* pDesc = {};


//UI 포지션 , 크기 체킹 
	_float m_fSizeX = { 100.f };
	_float m_fSizeY = { 100.f };
	_float m_fPosX = { 0.f };
	_float m_fPosY = { 0.f };

	_float4x4 m_ViewMatrix = {};
	_float4x4 m_ProjMatrix = {};

	class CCharacter* m_pMainPawn = { nullptr };
	class CCharacter* m_pSubPawn = { nullptr };

	CUI_Manager* m_pUI_Manager = { nullptr };

protected:
//Animation
	_bool m_bStart = { FALSE };
	_bool m_bAnimEnd = { FALSE };
	_vector m_vAnimStartPos = {};
	_vector m_vAnimTargetPos = {};
	_float m_fAnimDelayTiemr = { 0.f };

	_bool m_bCheck = { FALSE };
	_bool InManager = { FALSE };

protected:
	//해상도 변경
	_float2 m_vOffSetWinSize = { };
	_float2 m_vPrevWinSize = {};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

