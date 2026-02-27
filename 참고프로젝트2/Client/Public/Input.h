#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "AnimationEvent_Defines.h"
#include "FrameEvent_Manager.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

// 방향 입력 정의
enum DirectionInput {
    MOVEKEY_NEUTRAL,      // 중립
    MOVEKEY_UP,           // 위
    MOVEKEY_DOWN,         // 아래
    MOVEKEY_LEFT,         // 왼쪽
    MOVEKEY_RIGHT,        // 오른쪽
    MOVEKEY_UP_LEFT,      // 위 왼쪽
    MOVEKEY_UP_RIGHT,     // 위 오른쪽
    MOVEKEY_DOWN_LEFT,    // 아래 왼쪽
    MOVEKEY_DOWN_RIGHT    // 아래 오른쪽
};

// 버튼 입력 정의
enum ButtonInput {
    ATTACK_NONE,         // 버튼을 누르지 않음
    ATTACK_LIGHT,        // 펀치 버튼
    ATTACK_MEDIUM,          // 킥 버튼
    ATTACK_HEAVY,
    ATTACK_SPECIAL,
    ATTACK_GRAB,
    ATTACK_BENISHING,
    ATTACK_TRANSFORM,
};

// Input 클래스 정의 (방향 + 버튼을 포함)
class CInput {
public:
    DirectionInput direction;  // 플레이어가 입력한 방향
    ButtonInput button;        // 플레이어가 입력한 버튼


    _float frameTime{};             // 입력이 발생한 프레임 시간 (Optional)

   
    // 기본 생성자
   // CInput(DirectionInput dir = NEUTRAL, ButtonInput btn = NONE, int frame = 0)
   //     : direction(dir), button(btn), frameTime(frame) {}
   
    // 기본 생성자
    CInput(DirectionInput dir = MOVEKEY_NEUTRAL, ButtonInput btn = ATTACK_NONE)
        : direction(dir), button(btn) {}


    // 입력 비교를 위한 연산자 오버로딩
    bool operator==(const CInput& other) const {
        return direction == other.direction && button == other.button;
    }
  
};

END