#include "..\Public\Collider_Manager.h"

#include "GameObject.h"
#include "Component.h"
#include "GameInstance.h"
#include <chrono> // 시간 측정을 위해 추가
CCollider_Manager::CCollider_Manager()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

void CCollider_Manager::Update(_float fTimeDelta)
{
	Check_Collision(fTimeDelta);
}

HRESULT CCollider_Manager::Add_ColliderObject(COLLIDERGROUP eColliderGroup, CCollider* pColliderCom)
{
	if (pColliderCom == nullptr || eColliderGroup >= CG_END)
		return E_FAIL;

	Safe_AddRef(pColliderCom);

	m_Colliders[eColliderGroup].emplace_back(pColliderCom);

	return S_OK;
}


HRESULT CCollider_Manager::Check_Collision(_float fTimeDelta)
{
	// 이전 프레임의 충돌 결과 초기화
	m_CollisionResults.clear();

	// 모든 콜라이더의 m_isColl 플래그를 false로 초기화
	for (auto& group : m_Colliders)
	{
		for (auto& collider : group)
			collider->m_isColl = false;
	}


	// 충돌 쌍 생성
	vector<pair<CCollider*, CCollider*>> collisionPairs;

	// 람다 익명함수로 충돌 쌍 생성
	auto AddCollisionPairs = [&](COLLIDERGROUP group1, COLLIDERGROUP group2) {
		for (auto& collider1 : m_Colliders[group1]) {
			for (auto& collider2 : m_Colliders[group2]) {
				collisionPairs.emplace_back(collider1, collider2);
			}
		}
		};

	// 1P_BODY vs 2P_BODY
	AddCollisionPairs(CG_1P_BODY, CG_2P_BODY);

	// 1P_BODY vs 2P_SKILL
	AddCollisionPairs(CG_1P_BODY, CG_2P_Energy_Attack);

	// 1P_SKILL vs 2P_BODY
	AddCollisionPairs(CG_1P_Energy_Attack, CG_2P_BODY);

	// 1P_SKILL vs 2P_SKILL
	AddCollisionPairs(CG_1P_Energy_Attack, CG_2P_Energy_Attack);

	// 1P_RANGED_Attack vs 2P_Body
	AddCollisionPairs(CG_1P_Ranged_Attack, CG_2P_BODY);

	AddCollisionPairs(CG_1P_Ranged_Attack, CG_2P_Energy_Attack);

	AddCollisionPairs(CG_1P_Ranged_Attack, CG_2P_Ranged_Attack);

	AddCollisionPairs(CG_1P_Melee_Attack, CG_2P_BODY);

	AddCollisionPairs(CG_1P_BODY, CG_2P_Ranged_Attack);

	AddCollisionPairs(CG_1P_Energy_Attack, CG_2P_Ranged_Attack);

	AddCollisionPairs(CG_1P_BODY, CG_2P_Melee_Attack);

	// 1P_MELEE_ATTACK vs 2P_MELEE_ATTACK
	AddCollisionPairs(CG_1P_Melee_Attack, CG_2P_Melee_Attack);

	// 1P_RANGE_ATTACK vs 2P_MELEE_ATTACK
	AddCollisionPairs(CG_1P_Ranged_Attack, CG_2P_Melee_Attack);
	// 1P_MELEE_ATTACK vs 2P_RANGE_ATTACK
	AddCollisionPairs(CG_1P_Melee_Attack, CG_2P_Ranged_Attack);

	// 1P_REFLECT vs 2P_Energy_Attack
	AddCollisionPairs(CG_1P_REFLECT, CG_2P_Energy_Attack);
	// 1P_REFLECT vs 2P_Ranged_Attack
	AddCollisionPairs(CG_1P_REFLECT, CG_2P_Ranged_Attack);
	// 1P_REFLECT vs 2P_Melee_Attack
	AddCollisionPairs(CG_1P_REFLECT, CG_2P_Melee_Attack);

	// 1P_Energy_Attack vs 2P_REFLECT
	AddCollisionPairs(CG_1P_Energy_Attack, CG_2P_REFLECT);
	// 1P_Ranged_Attack vs 2P_REFLECT
	AddCollisionPairs(CG_1P_Ranged_Attack, CG_2P_REFLECT);
	// 1P_Melee_Attack vs 2P_REFLECT
	AddCollisionPairs(CG_1P_Melee_Attack, CG_2P_REFLECT);

	// 시간 측정 시작
	auto startTime = std::chrono::high_resolution_clock::now();

	// 스레드풀을 사용하여 작업 분할 및 충돌 검사 수행
	size_t numThreads = m_pGameInstance->Get_ThreadNumber();
	size_t totalPairs = collisionPairs.size();
	size_t pairsPerThread = (totalPairs + numThreads - 1) / numThreads; // 올림 계산

	//future<void>는 반환값이 없는 비동기 작업을 나타낸다
	vector<future<void>> futures;

	for (size_t t = 0; t < numThreads; ++t) {
		size_t startIdx = t * pairsPerThread;
		if (startIdx >= totalPairs)
			break;

		size_t endIdx = min(startIdx + pairsPerThread, totalPairs);

		// 각 스레드가 처리할 충돌 쌍 부분 벡터 생성
		vector<pair<CCollider*, CCollider*>> taskPairs(
			collisionPairs.begin() + startIdx,
			collisionPairs.begin() + endIdx
		);

		// 스레드풀에 작업 제출
		futures.emplace_back(m_pGameInstance->EnqueueTask(

			//Task의 람다함수
			[this, taskPairs, fTimeDelta]() {
				vector<pair<CCollider*, CCollider*>> localCollisions;

				for (const auto& pair : taskPairs) {
					if (IsColliding(pair.first, pair.second)) {
						localCollisions.emplace_back(pair);
					}
				}

				// 스레드 안전하게 결과를 저장
				{
					lock_guard<mutex> lock(m_ResultMutex);
					m_CollisionResults.insert(m_CollisionResults.end(), localCollisions.begin(), localCollisions.end());
				}
			}));
	}

	// 모든 작업 완료 대기
	for (auto& future : futures) {
		future.get();
	}

	// 시간 측정 종료
	auto endTime = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> elapsedTime = endTime - startTime;
	double elapsedSeconds = elapsedTime.count();

	// 충돌 결과 처리
	ProcessCollisionResults(fTimeDelta);

	return S_OK;
}


//충돌 결과 처리
void CCollider_Manager::ProcessCollisionResults(_float fTimeDelta)
{
	map<pair<CCollider*, CCollider*>, _bool> currentCollisions;

	// 그룹별로 충돌 쌍을 저장하는 컨테이너
	// 한번만 처리하기 때문에
	vector<pair<CCollider*, CCollider*>> Energy_1P_Skill_VS_Energy_2P_Skill_Collisions;
	vector<pair<CCollider*, CCollider*>> Energy_1P_Skill_VS_Body_2P_Collisions;
	vector<pair<CCollider*, CCollider*>> Body_1P_VS_Energy_2P_Skill_Collisions;

	vector<pair<CCollider*, CCollider*>> Ranged_Attack_1P_VS_Energy_2P_Skill_Collisions;
	vector<pair<CCollider*, CCollider*>> Energy_1P_Skill_VS_Ranged_2P_Skill_Collisions;

	vector<pair<CCollider*, CCollider*>> Reflect_1P_VS_Energy_2P_Skill_Collisions;
	vector<pair<CCollider*, CCollider*>> Energy_Attack_1P_VS_Reflect_2P_Collisions;


	// 각 스레드에서 취합한 충돌 결과가 있다면 m_CollisionResults에 저장
	for (const auto& pair : m_CollisionResults) {

		CCollider* colliderA = pair.first;
		CCollider* colliderB = pair.second;

		if (colliderA->GetMineGameObject()->m_bDead || colliderB->GetMineGameObject()->m_bDead)
			continue;

		//충돌 처리 완료
		colliderA->m_isColl = true;
		colliderB->m_isColl = true;

		// 조건을 bool 변수로 저장
		_bool is_1P_Body_Vs_2P_Energy_Skill = (colliderA->m_ColliderGroup == CG_1P_BODY && colliderB->m_ColliderGroup == CG_2P_Energy_Attack);
		_bool is_1P_Body_Vs_2P_Body = (colliderA->m_ColliderGroup == CG_1P_BODY && colliderB->m_ColliderGroup == CG_2P_BODY);
		_bool is_1P_Energy_Skill_Vs_2P_Energy_Skill = (colliderA->m_ColliderGroup == CG_1P_Energy_Attack && colliderB->m_ColliderGroup == CG_2P_Energy_Attack);
		_bool is_1P_Energy_Skill_Vs_2P_Body = (colliderA->m_ColliderGroup == CG_1P_Energy_Attack && colliderB->m_ColliderGroup == CG_2P_BODY);

		_bool is_1P_Ranged_Attack_Vs_2P_Body = (colliderA->m_ColliderGroup == CG_1P_Ranged_Attack && colliderB->m_ColliderGroup == CG_2P_BODY);
		_bool is_1P_Ranged_Attack_Vs_2P_Energy_Skill = (colliderA->m_ColliderGroup == CG_1P_Ranged_Attack && colliderB->m_ColliderGroup == CG_2P_Energy_Attack);
		_bool is_1P_Ranged_Attack_Vs_2P_Ranged_Attack = (colliderA->m_ColliderGroup == CG_1P_Ranged_Attack && colliderB->m_ColliderGroup == CG_2P_Ranged_Attack);
		_bool is_1P_Melee_Attack_Vs_2P_Body = (colliderA->m_ColliderGroup == CG_1P_Melee_Attack && colliderB->m_ColliderGroup == CG_2P_BODY);
		_bool is_1P_Body_Vs_2P_Ranged_Attack = (colliderA->m_ColliderGroup == CG_1P_BODY && colliderB->m_ColliderGroup == CG_2P_Ranged_Attack);
		_bool is_1P_Energy_Skill_Vs_2P_Ranged_Attack = (colliderA->m_ColliderGroup == CG_1P_Energy_Attack && colliderB->m_ColliderGroup == CG_2P_Ranged_Attack);
		_bool is_1P_Body_Vs_2P_Melee_Attack = (colliderA->m_ColliderGroup == CG_1P_BODY && colliderB->m_ColliderGroup == CG_2P_Melee_Attack);
		_bool is_1P_Melee_Vs_2P_Melee_Attack = (colliderA->m_ColliderGroup == CG_1P_Melee_Attack && colliderB->m_ColliderGroup == CG_2P_Melee_Attack);

		_bool is_1P_Range_Vs_2P_Melee_Attack = (colliderA->m_ColliderGroup == CG_1P_Ranged_Attack && colliderB->m_ColliderGroup == CG_2P_Melee_Attack);
		_bool is_1P_Melee_Vs_2P_Range_Attack = (colliderA->m_ColliderGroup == CG_1P_Melee_Attack && colliderB->m_ColliderGroup == CG_2P_Ranged_Attack);

		_bool is_1P_Reflect_Vs_2P_Energy_Attack = (colliderA->m_ColliderGroup == CG_1P_REFLECT && colliderB->m_ColliderGroup == CG_2P_Energy_Attack);
		_bool is_1P_Reflect_Vs_2P_Range_Attack = (colliderA->m_ColliderGroup == CG_1P_REFLECT && colliderB->m_ColliderGroup == CG_2P_Ranged_Attack);
		_bool is_1P_Reflect_Vs_2P_Melee_Attack = (colliderA->m_ColliderGroup == CG_1P_REFLECT && colliderB->m_ColliderGroup == CG_2P_Melee_Attack);

		_bool is_1P_Energy_Attack_Vs_2P_Reflect = (colliderA->m_ColliderGroup == CG_1P_Energy_Attack && colliderB->m_ColliderGroup == CG_2P_REFLECT);
		_bool is_1P_Range_Attack_Vs_2P_Reflect = (colliderA->m_ColliderGroup == CG_1P_Ranged_Attack && colliderB->m_ColliderGroup == CG_2P_REFLECT);
		_bool is_1P_Melee_Attack_Vs_2P_Reflect = (colliderA->m_ColliderGroup == CG_1P_Melee_Attack && colliderB->m_ColliderGroup == CG_2P_REFLECT);


		// 충돌 그룹에 따른 처리


		if (is_1P_Body_Vs_2P_Body)
		{
			//1P_Body VS 2P_Body
			Process_1P_Body_2P_Body(pair, fTimeDelta, currentCollisions);
		}

		if (is_1P_Energy_Skill_Vs_2P_Energy_Skill)
		{
			//1P_Skill VS 2P_Skill
			Energy_1P_Skill_VS_Energy_2P_Skill_Collisions.push_back(pair);
		}

		if (is_1P_Ranged_Attack_Vs_2P_Ranged_Attack)
		{
			Process_1P_Ranged_Skill_2P_Ranged_Skill(pair, fTimeDelta, currentCollisions);
		}

		if (is_1P_Melee_Vs_2P_Melee_Attack)
		{
			Process_1P_Melee_2P_Melee_Skill(pair, fTimeDelta, currentCollisions);
		}






		if (is_1P_Body_Vs_2P_Energy_Skill)
		{
			//1P_Body VS 2P_Skill
			Body_1P_VS_Energy_2P_Skill_Collisions.push_back(pair);
		}

		if (is_1P_Energy_Skill_Vs_2P_Body)
		{
			//1P_Skill VS 2P_Body
			Energy_1P_Skill_VS_Body_2P_Collisions.push_back(pair);
		}





		if (is_1P_Ranged_Attack_Vs_2P_Body)
		{
			Process_1P_Ranged_Skill_2P_Body(pair, fTimeDelta, currentCollisions);
		}

		if (is_1P_Body_Vs_2P_Ranged_Attack)
		{
			Process_1P_Body_2P_Ranged_Skill(pair, fTimeDelta, currentCollisions);
		}





		if (is_1P_Melee_Attack_Vs_2P_Body)
		{
			Process_1P_Melee_Skill_2P_Body(pair, fTimeDelta, currentCollisions);
		}

		if (is_1P_Body_Vs_2P_Melee_Attack)
		{
			Process_1P_Body_2P_Melee_Skill(pair, fTimeDelta, currentCollisions);
		}





		if (is_1P_Energy_Skill_Vs_2P_Ranged_Attack)
		{
			Energy_1P_Skill_VS_Ranged_2P_Skill_Collisions.push_back(pair);
		}

		if (is_1P_Ranged_Attack_Vs_2P_Energy_Skill)
		{
			Ranged_Attack_1P_VS_Energy_2P_Skill_Collisions.push_back(pair);
		}




		if (is_1P_Range_Vs_2P_Melee_Attack)
		{
			Process_1P_Range_2P_Melee_Skill(pair, fTimeDelta, currentCollisions);
		}

		if (is_1P_Melee_Vs_2P_Range_Attack)
		{
			Process_1P_Melee_2P_Range_Skill(pair, fTimeDelta, currentCollisions);
		}

		if (is_1P_Reflect_Vs_2P_Energy_Attack)
		{
			Reflect_1P_VS_Energy_2P_Skill_Collisions.push_back(pair);
		}

		if (is_1P_Reflect_Vs_2P_Range_Attack)
		{
			Process_1P_Reflect_2P_Range_Skill(pair, fTimeDelta, currentCollisions);
		}

		if (is_1P_Reflect_Vs_2P_Melee_Attack)
		{
			Process_1P_Reflect_2P_Melee_Skill(pair, fTimeDelta, currentCollisions);
		}

		if (is_1P_Energy_Attack_Vs_2P_Reflect)
		{
			Energy_Attack_1P_VS_Reflect_2P_Collisions.push_back(pair);
		}

		if (is_1P_Range_Attack_Vs_2P_Reflect)
		{
			Process_1P_Range_2P_Reflect(pair, fTimeDelta, currentCollisions);
		}

		if (is_1P_Melee_Attack_Vs_2P_Reflect)
		{
			Process_1P_Melee_2P_Reflect(pair, fTimeDelta, currentCollisions);
		}

	}

	// 각 그룹별로 한 번만 처리
	if (!Energy_1P_Skill_VS_Energy_2P_Skill_Collisions.empty())
	{
		Process_1P_Energy_Skill_2P_Energy_Skill_Group(Energy_1P_Skill_VS_Energy_2P_Skill_Collisions, fTimeDelta, currentCollisions);
	}

	// 각 그룹별로 한 번만 처리
	if (!Body_1P_VS_Energy_2P_Skill_Collisions.empty())
	{
		Process_1P_Body_2P_Energy_Skill_Group(Body_1P_VS_Energy_2P_Skill_Collisions, fTimeDelta, currentCollisions);
	}

	if (!Energy_1P_Skill_VS_Body_2P_Collisions.empty())
	{
		Process_1P_Energy_Skill_2P_Body_Group(Energy_1P_Skill_VS_Body_2P_Collisions, fTimeDelta, currentCollisions);
	}

	if (!Ranged_Attack_1P_VS_Energy_2P_Skill_Collisions.empty())
	{
		Process_1P_Ranged_Skill_2P_Energy_Skill_Group(Ranged_Attack_1P_VS_Energy_2P_Skill_Collisions, fTimeDelta, currentCollisions);
	}

	if (!Energy_1P_Skill_VS_Ranged_2P_Skill_Collisions.empty())
	{
		Process_1P_Energy_Skill_2P_Ranged_Skill_Group(Energy_1P_Skill_VS_Ranged_2P_Skill_Collisions, fTimeDelta, currentCollisions);
	}

	if (!Reflect_1P_VS_Energy_2P_Skill_Collisions.empty())
	{
		Process_1P_Reflect_2P_Energy_Skill_Group(Reflect_1P_VS_Energy_2P_Skill_Collisions, fTimeDelta, currentCollisions);
	}
	if (!Energy_Attack_1P_VS_Reflect_2P_Collisions.empty())
	{
		Process_1P_Energy_2P_Reflect_Group(Energy_Attack_1P_VS_Reflect_2P_Collisions, fTimeDelta, currentCollisions);
	}


	// 충돌 히스토리 업데이트
	m_CollisionHistory = currentCollisions;
}


void CCollider_Manager::Process_1P_Body_2P_Body(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
		pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
	}
	else {
		// 충돌 지속
		pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
	}

	// 현재 충돌 상태 업데이트
	currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;

	// 이전 프레임에서 1P Body와 2P Body 간의 충돌 중 이번 프레임에서 충돌하지 않은 경우 처리
	for (auto& pair : m_CollisionHistory) {
		// 충돌 쌍이 1P Body와 2P Body인지 확인
		CCollider* colliderA = pair.first.first;
		CCollider* colliderB = pair.first.second;

		if (colliderA->m_ColliderGroup == CG_1P_BODY && colliderB->m_ColliderGroup == CG_2P_BODY)
		{

			if (currentCollisions.find(pair.first) == currentCollisions.end()) {
				colliderA->OnCollisionExit(colliderB);
				colliderB->OnCollisionExit(colliderA);
				break;
			}
		}
	}
}

void CCollider_Manager::Process_1P_Energy_Skill_2P_Energy_Skill_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	// 첫 번째 충돌 쌍만 사용하여 처리
	if (!collisions.empty())
	{
		const auto& pairCollider = collisions.front();

		if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
			pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
			pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
		}
		else {
			// 충돌 지속
			pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
			pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
		}

		// 현재 충돌 상태 업데이트
		currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;
	}
}

void CCollider_Manager::Process_1P_Body_2P_Energy_Skill_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	// 첫 번째 충돌 쌍만 사용하여 처리
	if (!collisions.empty())
	{
		const auto& pairCollider = collisions.front();

		if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
			pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
			pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
		}
		else {
			// 충돌 지속
			pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
			pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
		}

		// 현재 충돌 상태 업데이트
		currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;
	}
}

void CCollider_Manager::Process_1P_Energy_Skill_2P_Body_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	// 첫 번째 충돌 쌍만 사용하여 처리
	if (!collisions.empty())
	{
		const auto& pairCollider = collisions.front();

		if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
			pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
			pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
		}
		else {
			// 충돌 지속
			pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
			pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
		}

		// 현재 충돌 상태 업데이트
		currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;
	}
}

void CCollider_Manager::Process_1P_Ranged_Skill_2P_Body(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
		pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
	}
	else {
		// 충돌 지속
		pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
	}

	// 현재 충돌 상태 업데이트
	currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;

	// 이전 프레임에서 충돌한 것들 중 이번 프레임에서 충돌하지 않은 경우 처리
	for (auto& pair : m_CollisionHistory) {
		CCollider* colliderA = pair.first.first;
		CCollider* colliderB = pair.first.second;

		if (colliderA->m_ColliderGroup == CG_1P_Ranged_Attack && colliderB->m_ColliderGroup == CG_2P_BODY)
		{
			if (currentCollisions.find(pair.first) == currentCollisions.end()) {
				colliderA->OnCollisionExit(colliderB);
				colliderB->OnCollisionExit(colliderA);
				break;
			}
		}
	}

	Destroy_Reserve(pairCollider.first);
}

void CCollider_Manager::Process_1P_Ranged_Skill_2P_Energy_Skill_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	// 첫 번째 충돌 쌍만 사용하여 처리
	if (!collisions.empty())
	{
		const auto& pairCollider = collisions.front();

		if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
			pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
			pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
		}
		else {
			// 충돌 지속
			pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
			pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
		}

		// 현재 충돌 상태 업데이트
		currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;

		//Melee_Attack
		Destroy_Reserve(pairCollider.first);
	}
}

void CCollider_Manager::Process_1P_Ranged_Skill_2P_Ranged_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
		pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
	}
	else {
		// 충돌 지속
		pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
	}

	// 현재 충돌 상태 업데이트
	currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;

	// 이전 프레임에서 충돌한 것들 중 이번 프레임에서 충돌하지 않은 경우 처리
	for (auto& pair : m_CollisionHistory) {
		CCollider* colliderA = pair.first.first;
		CCollider* colliderB = pair.first.second;

		if (colliderA->m_ColliderGroup == CG_1P_Ranged_Attack && colliderB->m_ColliderGroup == CG_2P_Ranged_Attack)
		{
			if (currentCollisions.find(pair.first) == currentCollisions.end()) {
				colliderA->OnCollisionExit(colliderB);
				colliderB->OnCollisionExit(colliderA);
				break;
			}
		}
	}

	Destroy_Reserve(pairCollider.first);
	Destroy_Reserve(pairCollider.second);
}

void CCollider_Manager::Process_1P_Melee_Skill_2P_Body(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
		pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
	}
	else {
		// 충돌 지속
		pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
	}

	// 현재 충돌 상태 업데이트
	currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;

	// 이전 프레임에서 충돌한 것들 중 이번 프레임에서 충돌하지 않은 경우 처리
	for (auto& pair : m_CollisionHistory) {
		CCollider* colliderA = pair.first.first;
		CCollider* colliderB = pair.first.second;

		if (colliderA->m_ColliderGroup == CG_1P_Melee_Attack && colliderB->m_ColliderGroup == CG_2P_BODY)
		{
			if (currentCollisions.find(pair.first) == currentCollisions.end()) {
				colliderA->OnCollisionExit(colliderB);
				colliderB->OnCollisionExit(colliderA);
				break;
			}
		}
	}

	Destroy_Reserve(pairCollider.first);
}

void CCollider_Manager::Process_1P_Body_2P_Ranged_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
		pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
	}
	else {
		// 충돌 지속
		pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
	}

	// 현재 충돌 상태 업데이트
	currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;

	// 이전 프레임에서 충돌한 것들 중 이번 프레임에서 충돌하지 않은 경우 처리
	for (auto& pair : m_CollisionHistory) {
		CCollider* colliderA = pair.first.first;
		CCollider* colliderB = pair.first.second;

		if (colliderA->m_ColliderGroup == CG_1P_BODY && colliderB->m_ColliderGroup == CG_2P_Ranged_Attack)
		{
			if (currentCollisions.find(pair.first) == currentCollisions.end()) {
				colliderA->OnCollisionExit(colliderB);
				colliderB->OnCollisionExit(colliderA);
				break;
			}
		}
	}

	Destroy_Reserve(pairCollider.second);
}

void CCollider_Manager::Process_1P_Energy_Skill_2P_Ranged_Skill_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	// 첫 번째 충돌 쌍만 사용하여 처리
	if (!collisions.empty())
	{
		const auto& pairCollider = collisions.front();

		if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
			pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
			pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
		}
		else {
			// 충돌 지속
			pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
			pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
		}

		// 현재 충돌 상태 업데이트
		currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;

		//Ranged_Attack
		Destroy_Reserve(pairCollider.second);
	}
}

void CCollider_Manager::Process_1P_Body_2P_Melee_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
		pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
	}
	else {
		// 충돌 지속
		pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
	}

	// 현재 충돌 상태 업데이트
	currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;

	// 이전 프레임에서 충돌한 것들 중 이번 프레임에서 충돌하지 않은 경우 처리
	for (auto& pair : m_CollisionHistory) {
		CCollider* colliderA = pair.first.first;
		CCollider* colliderB = pair.first.second;

		if (colliderA->m_ColliderGroup == CG_1P_BODY && colliderB->m_ColliderGroup == CG_2P_Melee_Attack)
		{
			if (currentCollisions.find(pair.first) == currentCollisions.end()) {
				colliderA->OnCollisionExit(colliderB);
				colliderB->OnCollisionExit(colliderA);
				break;
			}
		}
	}

	Destroy_Reserve(pairCollider.second);
}

void CCollider_Manager::Process_1P_Melee_2P_Melee_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
		pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
	}
	else {
		// 충돌 지속
		pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
	}

	// 현재 충돌 상태 업데이트
	currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;

	// 이전 프레임에서 충돌한 것들 중 이번 프레임에서 충돌하지 않은 경우 처리
	for (auto& pair : m_CollisionHistory) {
		CCollider* colliderA = pair.first.first;
		CCollider* colliderB = pair.first.second;

		if (colliderA->m_ColliderGroup == CG_1P_Melee_Attack && colliderB->m_ColliderGroup == CG_2P_Melee_Attack)
		{
			if (currentCollisions.find(pair.first) == currentCollisions.end()) {
				colliderA->OnCollisionExit(colliderB);
				colliderB->OnCollisionExit(colliderA);
				break;
			}
		}
	}

	Destroy_Reserve(pairCollider.first);
	Destroy_Reserve(pairCollider.second);
}

void CCollider_Manager::Process_1P_Range_2P_Melee_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
		pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
	}
	else {
		// 충돌 지속
		pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
	}

	// 현재 충돌 상태 업데이트
	currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;

	// 이전 프레임에서 충돌한 것들 중 이번 프레임에서 충돌하지 않은 경우 처리
	for (auto& pair : m_CollisionHistory) {
		CCollider* colliderA = pair.first.first;
		CCollider* colliderB = pair.first.second;

		if (colliderA->m_ColliderGroup == CG_1P_Ranged_Attack && colliderB->m_ColliderGroup == CG_2P_Melee_Attack)
		{
			if (currentCollisions.find(pair.first) == currentCollisions.end()) {
				colliderA->OnCollisionExit(colliderB);
				colliderB->OnCollisionExit(colliderA);
				break;
			}
		}
	}
}

void CCollider_Manager::Process_1P_Melee_2P_Range_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
		pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
	}
	else {
		// 충돌 지속
		pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
	}

	// 현재 충돌 상태 업데이트
	currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;

	// 이전 프레임에서 충돌한 것들 중 이번 프레임에서 충돌하지 않은 경우 처리
	for (auto& pair : m_CollisionHistory) {
		CCollider* colliderA = pair.first.first;
		CCollider* colliderB = pair.first.second;

		if (colliderA->m_ColliderGroup == CG_1P_Melee_Attack && colliderB->m_ColliderGroup == CG_2P_Ranged_Attack)
		{
			if (currentCollisions.find(pair.first) == currentCollisions.end()) {
				colliderA->OnCollisionExit(colliderB);
				colliderB->OnCollisionExit(colliderA);
				break;
			}
		}
	}
}

void CCollider_Manager::Process_1P_Reflect_2P_Energy_Skill_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	// 첫 번째 충돌 쌍만 사용하여 처리
	if (!collisions.empty())
	{
		const auto& pairCollider = collisions.front();

		if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
			pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
			pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
		}
		else {
			// 충돌 지속
			pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
			pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
		}

		// 현재 충돌 상태 업데이트
		currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;
	}
}

void CCollider_Manager::Process_1P_Reflect_2P_Range_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
		pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
	}
	else {
		// 충돌 지속
		pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
	}

	// 현재 충돌 상태 업데이트
	currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;

	// 이전 프레임에서 충돌한 것들 중 이번 프레임에서 충돌하지 않은 경우 처리
	for (auto& pair : m_CollisionHistory) {
		CCollider* colliderA = pair.first.first;
		CCollider* colliderB = pair.first.second;

		if (colliderA->m_ColliderGroup == CG_1P_REFLECT && colliderB->m_ColliderGroup == CG_2P_Ranged_Attack)
		{
			if (currentCollisions.find(pair.first) == currentCollisions.end()) {
				colliderA->OnCollisionExit(colliderB);
				colliderB->OnCollisionExit(colliderA);
				break;
			}
		}
	}
}

void CCollider_Manager::Process_1P_Reflect_2P_Melee_Skill(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
		pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
	}
	else {
		// 충돌 지속
		pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
	}

	// 현재 충돌 상태 업데이트
	currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;

	// 이전 프레임에서 충돌한 것들 중 이번 프레임에서 충돌하지 않은 경우 처리
	for (auto& pair : m_CollisionHistory) {
		CCollider* colliderA = pair.first.first;
		CCollider* colliderB = pair.first.second;

		if (colliderA->m_ColliderGroup == CG_1P_REFLECT && colliderB->m_ColliderGroup == CG_2P_Melee_Attack)
		{
			if (currentCollisions.find(pair.first) == currentCollisions.end()) {
				colliderA->OnCollisionExit(colliderB);
				colliderB->OnCollisionExit(colliderA);
				break;
			}
		}
	}
}

void CCollider_Manager::Process_1P_Energy_2P_Reflect_Group(const vector<pair<CCollider*, CCollider*>>& collisions, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	// 첫 번째 충돌 쌍만 사용하여 처리
	if (!collisions.empty())
	{
		const auto& pairCollider = collisions.front();

		if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
			pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
			pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
		}
		else {
			// 충돌 지속
			pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
			pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
		}

		// 현재 충돌 상태 업데이트
		currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;
	}
}


void CCollider_Manager::Process_1P_Range_2P_Reflect(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
		pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
	}
	else {
		// 충돌 지속
		pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
	}

	// 현재 충돌 상태 업데이트
	currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;

	// 이전 프레임에서 충돌한 것들 중 이번 프레임에서 충돌하지 않은 경우 처리
	for (auto& pair : m_CollisionHistory) {
		CCollider* colliderA = pair.first.first;
		CCollider* colliderB = pair.first.second;

		if (colliderA->m_ColliderGroup == CG_1P_Ranged_Attack && colliderB->m_ColliderGroup == CG_2P_REFLECT)
		{
			if (currentCollisions.find(pair.first) == currentCollisions.end()) {
				colliderA->OnCollisionExit(colliderB);
				colliderB->OnCollisionExit(colliderA);
				break;
			}
		}
	}
}

void CCollider_Manager::Process_1P_Melee_2P_Reflect(pair<CCollider*, CCollider*> pairCollider, _float fTimeDelta, map<pair<CCollider*, CCollider*>, _bool>& currentCollisions)
{
	if (m_CollisionHistory.find(pairCollider) == m_CollisionHistory.end() || !m_CollisionHistory[pairCollider]) {
		pairCollider.first->OnCollisionEnter(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionEnter(pairCollider.first, fTimeDelta);
	}
	else {
		// 충돌 지속
		pairCollider.first->OnCollisionStay(pairCollider.second, fTimeDelta);
		pairCollider.second->OnCollisionStay(pairCollider.first, fTimeDelta);
	}

	// 현재 충돌 상태 업데이트
	currentCollisions[make_pair(pairCollider.first, pairCollider.second)] = true;

	// 이전 프레임에서 충돌한 것들 중 이번 프레임에서 충돌하지 않은 경우 처리
	for (auto& pair : m_CollisionHistory) {
		CCollider* colliderA = pair.first.first;
		CCollider* colliderB = pair.first.second;

		if (colliderA->m_ColliderGroup == CG_1P_Melee_Attack && colliderB->m_ColliderGroup == CG_2P_REFLECT)
		{
			if (currentCollisions.find(pair.first) == currentCollisions.end()) {
				colliderA->OnCollisionExit(colliderB);
				colliderB->OnCollisionExit(colliderA);
				break;
			}
		}
	}
}


_bool CCollider_Manager::IsColliding(CCollider* _pSourCollider, CCollider* _pDestCollider)
{
	_bool isCol = _pSourCollider->isCollision(_pDestCollider);

	return isCol;
}

HRESULT CCollider_Manager::Release_Collider(const CCollider* targetCollider)
{
	// 먼저, m_CollisionHistory에서 해당 콜라이더와 관련된 모든 충돌 정보를 제거하거나 OnCollisionExit 이벤트를 호출합니다.
	for (auto it = m_CollisionHistory.begin(); it != m_CollisionHistory.end(); )
	{
		if (it->first.first == targetCollider || it->first.second == targetCollider)
		{
			// 충돌 정보 제거
			it = m_CollisionHistory.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (int i = CG_1P_BODY; i < CG_END; ++i) {

		auto it = find(m_Colliders[i].begin(), m_Colliders[i].end(), targetCollider);
		if (it != m_Colliders[i].end()) {
			CCollider* collider = *it;
			if (!collider->m_bDead)
			{
				Safe_Release(collider);

			}
			m_Colliders[i].erase(it);

			return S_OK; // 성공적으로 해제 및 삭제
		}
	}

	return E_FAIL; // 해당 collider를 찾지 못함
}

HRESULT CCollider_Manager::Destory_ColliderGroup()
{
	// 먼저 개별 콜라이더 삭제
	for (auto& pCollider : m_Destory_Reserve_Collider)
	{
		Release_Collider(pCollider);
	}

	m_Destory_Reserve_Collider.clear();

	// 그룹 전체 삭제
	for (auto& group : m_Destroy_Reserve_Collider_Group)
	{
		for (auto& pCollider : m_Colliders[group])
			Safe_Release(pCollider);
		m_Colliders[group].clear();
	}
	m_Destroy_Reserve_Collider_Group.clear();
	return S_OK;
}

void CCollider_Manager::Destroy_Reserve(COLLIDERGROUP eRenderGroup)
{
	// 리스트에 이미 존재하는지 확인
	if (find(m_Destroy_Reserve_Collider_Group.begin(), m_Destroy_Reserve_Collider_Group.end(), eRenderGroup) == m_Destroy_Reserve_Collider_Group.end())
	{
		// 존재하지 않으면 추가
		m_Destroy_Reserve_Collider_Group.push_back(eRenderGroup);
	}
}

void CCollider_Manager::Destroy_Reserve(CCollider* pCollider)
{
	// 이미 리스트에 존재하는지 확인
	if (find(m_Destory_Reserve_Collider.begin(), m_Destory_Reserve_Collider.end(), pCollider) == m_Destory_Reserve_Collider.end())
	{
		m_Destory_Reserve_Collider.push_back(pCollider);
	}
}




CCollider_Manager* CCollider_Manager::Create()
{
	return new CCollider_Manager();
}

void CCollider_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < CG_END; i++)
	{
		for (auto& pCollider : m_Colliders[i])
			Safe_Release(pCollider);
		m_Colliders[i].clear();
	}

	Safe_Release(m_pGameInstance);
}
