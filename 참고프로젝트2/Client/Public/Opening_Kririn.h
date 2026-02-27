#pragma once

#include "Client_Defines.h"
#include "AnimationEvent_Defines.h"
#include "FrameEvent_Manager.h"
#include "Character.h"

#include "Hit_MeleeAttack.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

//Prototype_GameObject_Opening_Kririn

class COpening_Kririn final : public CCharacter
{
public:
	typedef struct : CGameObject::GAMEOBJECT_DESC
	{
		_wstring strModelName;
	}Play_Hit_DESC;
	enum AnimationIndex
	{

		ANIME_IDLE = 2


		
	};
	enum AnimationCount
	{
		COUNT_ATTACK_MEDIUM = 0,
		//COUNT_ATTACK_CROUCH_MEDUIM,
		COUNT_ATTACK_AUTO_MEDIUMTOHEAVY,
		COUNT_ATTACK_SPECIAL,
		COUNT_ATTACK_236SPECIAL_UP,
		COUNT_ATTACK_236SPECIAL_RIGHT,
		COUNT_ATTACK_236SPECIAL_DOWN,
		COUNT_ATTACK_236_LIGHT,
		COUNT_ATTACK_236_MEDIUM,
		COUNT_ATTACK_236_HEAVY,
		COUNT_ATTACK_GRAB,
		COUNT_ATTACK_BENISHING,
		COUNT_END
	};

private:
	COpening_Kririn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COpening_Kririn(const COpening_Kririn& Prototype);
	virtual ~COpening_Kririn() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Player_Update(_float fTimeDelta) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;






	virtual _short Check_bCurAnimationisCanChase() override;


	_bool* Get_pbAttackCount() { return m_bAttackCount; };

	virtual void Reset_AttackCount() override;

	virtual void Gravity(_float fTimeDelta) override;

	virtual void AttackEvent(_int iAttackEvent, _int AddEvent = 0)override;
	virtual void Play_Sound(_uint SoundName, _bool bisLoof, _float fvolume)override;
	virtual void Play_Group_Sound(_uint groupKey, _bool loop, _float volume)override;



	void Add_YellowLight();
	void Add_BlueLight();



	void Set_Delete();


	

private:
	
	_bool m_bAttackCount[COUNT_END] = { true };
	CTexture* m_pDiffuseTextureCom = { nullptr };
	CTexture* m_pDecalTextureCom = { nullptr };
	CTexture* m_pIlmTextureCom = { nullptr };


	//_bool m_b214Medium = { 0 };


	//_bool m_bInvisible = { false };


	//_bool m_bUltimate_Air


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static COpening_Kririn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END