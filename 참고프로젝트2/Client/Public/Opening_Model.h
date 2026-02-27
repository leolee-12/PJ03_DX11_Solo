#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "UI_Define.h"
#include "AnimationEvent_Defines.h"
#include "FrameEvent_Manager.h"

#include "Input.h"
#include <functional>

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)



class COpening_Model : public CGameObject
{
public:



public:

	typedef struct : CGameObject::GAMEOBJECT_DESC
	{
		wstring strModelname = {};
		_uint iAnimationIndex = {};
		_vector vStartPostion = {};

	}Opening_Model_DESC;


protected:
	COpening_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COpening_Model(const COpening_Model& Prototype);
	virtual ~COpening_Model() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Player_Update(_float fTimeDelta) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;



	//애니메이션
	virtual _bool Character_Play_Animation(_float fTimeDelta);
	

	
	virtual void Set_Animation(_uint iAnimationIndex, _bool bloof = false);
	


	virtual void Set_CurrentAnimationPositionJump(_float fAnimationPosition);

	//키프레임 이벤트
	virtual void ProcessEventsFramesZero(_uint characterIndex, _uint animationIndex);
	virtual void ProcessEventsBetweenFrames2(int characterIndex, int animationIndex, float prevFrame, float currentFrame);

	//키프레임 이벤트 파생
	void Create_Effect(_int iEffectIndex);



	void FlipDirection(_int iDirection = 0);
	_int Get_iDirection() { return m_iLookDirection; };
	
	virtual void AttackEvent(_int iAttackEvent, _int AddEvent = 0) {};
	virtual void Play_Sound(_uint SoundName, _bool bisLoof, _float fvolume) {};
	virtual void Play_Group_Sound(_uint groupKey, _bool loop, _float volume) {};




	void Add_Move(_float2 fMovement);




protected:
	_float4x4 Make_BoneMatrix(char* BoneName);

	_float4x4 Make_BoneMatrix_Offset(char* BoneName, _float2 fOffset, _bool bFlipDirection = false);
	_float4x4 Character_Make_Matrix(_float2 fOffset = { 0,0 }, _bool bFlipDirection = false);

public:
	void		Character_Make_BoneEffect_Offset(char* BoneName, _wstring strEffectName, _float2 fOffset = { 0.f,0.f }, _bool bFlipDirection = false);
	class CEffect_Layer* Character_Make_BoneEffect(char* BoneName, _wstring strEffectName);
	class CEffect_Layer* Character_Make_Effect(_wstring strEffectName, _float2 fOffset = { 0,0 }, _bool bFlipDirection = false);

	void Character_Create_Distortion(_float3 vDir = { 0.f,1.f,0.f }, _float2 vOffSetPos = {0.f,0.f}, _float2 vOffSetScale = { 1.f,1.f }, _float fLifeTime = { 0.3f });
	void Character_Create_Distortion(_float3 vDir, _float3 vOffSetPos, _float2 vOffSetScale = { 1.f,1.f }, _float fLifeTime = { 0.3f });



	void Set_LoofAnimationCreate(_wstring strEffectName, _float fMaxTime, _float fPeriodTime, _float2 fOffset = { 0.f,0.f }, _bool bFlipDirection = false);
	void Update_LoofAnimationCreate(_float fTimeDelta);

	const _float4x4* Get_pTransformMatrix();

public:
	
protected:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pOutLineCom = { nullptr };
	CTexture* m_p2PTextureCom = { nullptr };
	CTexture* m_p1PTextureCom = { nullptr };
	CTexture* m_pDecalTextureCom = { nullptr };
	CTexture* m_pDetailTextureCom = { nullptr };
	
	_wstring				m_strModelName{};

	//_uint					m_iMoveGrade = {};
	//_uint					m_iCurMoveIndex{};
	//커맨드 만들면 스택형으로 넣어두고 0.3초안에 지우기

	//피격 스턴과 다름
	_float					m_fAccAnimationLock{};
	_float					m_fMaxAnimationLock{};
	_bool					m_bAnimationLock{};

	CHARACTER_INDEX			m_eCharacterIndex = { OPENING_KRN };

	FrameEventMap*			 m_pFrameEvent = { nullptr };
	_bool					m_bMotionPlaying = false;

	vector<CInput> inputBuffer;

	_int	m_iLookDirection = { 1 };


	//index,시간
	pair<_uint, _float>		m_iNextAnimation{ 0,0 };
	_float					m_fNextAnimationCurrentPosition = {};




	//피격시 Y축 이동을 대비해 xy로 만듦.  각도별로 되어있으니 그냥 애니메이션으로 처리할까 생각도 듦
	_float2 m_fImpuse = { 0.f,0.f };

	_bool m_bInvisible = { false };


protected:
	CUI_Define::PLAYER_ID				m_eCharacterID = {};
	CUI_Define::PLAYER_SLOT				m_ePlayerSlot = { CUI_Define::SLOT_END };
	class CUI_Manager* m_pUI_Manager = { nullptr };
	class CEffect_Manager* m_pEffect_Manager = { nullptr };

	string m_strName = "HIT_1_1";
	_float4		m_vDyingPosition = { 0.f,0.f,0.f,1.f };
	//_float4x4 m_IdentityMatrix;
private:
	HRESULT Ready_Components(Opening_Model_DESC* pDesc);
public:
	HRESULT Bind_ShaderResources();


	CUI_Define::PLAYER_ID Get_CharacterID() { return m_eCharacterID; };




public:
	static COpening_Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


};



END