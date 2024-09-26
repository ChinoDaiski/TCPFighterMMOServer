#pragma once

#include "Object.h"

#define FLAG_MOVING         0x01
#define FLAG_DEAD           0x02

void SendCreationPacketBetween(CObject*, CObject*);
void SendDestructionPacketBetween(CObject*, CObject*);

class CPlayer : public CObject {
public:
    explicit CPlayer(UINT16 _x = 0, UINT16 _y = 0, UINT8 _direction = 0, UINT8 _hp = 0) noexcept;
    virtual ~CPlayer();

    // 기타 멤버 함수 선언
    virtual void Update(void) override;
    virtual void LateUpdate(void) override;
    virtual void Move(void) override;

public:
    void Init(UINT16 _x, UINT16 _y, UINT8 _direction, UINT8 _hp);

public:
    // 나중에 플레이어를 제외한 다른 오브젝트가 추가되면 이 함수들을 CObject에 넣고, 상속받은 class들이 오버라이딩해서 관련 내용을 작성하면 된다.
    // 일단 플레이어만 있으니 이렇게 작성.
    friend void SendCreationPacketBetween(CObject*, CObject*);
    friend void SendDestructionPacketBetween(CObject*, CObject*);

public:
    // Getter 함수
    constexpr UINT8 GetHp() const { return m_hp; }
    constexpr UINT8 GetDirection() const { return m_direction; }
    constexpr UINT8 GetFacingDirection() const { return m_facingDirection; }

    // Setter 함수
    void SetHp(int _hp) { m_hp = _hp; }
    void SetDirection(int _direction);
    void Damaged(int _hp);
    void SetSpeed(UINT8 speedX, UINT8 speedY);

public:
    void SetFlag(UINT8 flag, bool bOnOff);
    void ToggleFlag(UINT8 flag);
    bool isBitSet(UINT8 flag);

public:
    void SetFlagField(UINT8 pField);

private:
    UINT8 m_hp;          // 체력
    UINT8 m_direction;   // 방향
    UINT8 m_facingDirection;    // 캐릭터가 바라보고 있는 방향
    UINT8 m_speedX;     // 스피드
    UINT8 m_speedY;
    UINT8 m_FlagField;  // 행동 관련 flagField;
};