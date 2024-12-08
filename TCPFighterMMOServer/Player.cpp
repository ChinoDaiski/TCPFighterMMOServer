
#include "pch.h"
#include "Player.h"
#include "SessionManager.h"
#include "MakePacket.h"
#include "SectorManager.h"

#include <sstream>

int dir[8][2] = {
    {-1, 0},	// LL
    {-1, -1},	// LU
    {0, -1},	// UU
    {1, -1},	// RU
    {1, 0},		// RR
    {1, 1},		// RD
    {0, 1},		// DD
    {-1, 1}	    // LD
};



CPlayer::CPlayer(UINT16 _x, UINT16 _y, UINT8 _direction, UINT8 _hp) noexcept
	: CObject{ _x, _y }, m_hp{ _hp }, m_direction{ _direction }
{
}

CPlayer::~CPlayer()
{
}

void CPlayer::Update(void)
{ 
    __super::Update();

    if (!m_FlagField)
        return;

    if (m_FlagField & FLAG_MOVING)
        Move();
}

void CPlayer::LateUpdate(void)
{
    __super::LateUpdate();
}

void CPlayer::Move(void)
{
    // 이동하는 위치 계산
    int resultX = m_x + (dir[m_direction][0] * m_speedX);
    int resultY = m_y + (dir[m_direction][1] * m_speedY);

    // 범위 검사
    if (
        (resultX >= dfRANGE_MOVE_RIGHT) ||
        (resultX < dfRANGE_MOVE_LEFT) ||
        (resultY >= dfRANGE_MOVE_BOTTOM) ||
        (resultY < dfRANGE_MOVE_TOP)
        )
        return;
    // 범위 검사 통과시
    else
    {
        // 위치 적용
        m_x = resultX;
        m_y = resultY;
    }
}

void CPlayer::Init(UINT16 _x, UINT16 _y, UINT8 _direction, UINT8 _hp)
{
    m_x = _x;
    m_y = _y;
    m_pSession = nullptr;
    m_bDead = false;
    m_hp = _hp;
    m_direction = _direction;
    m_facingDirection = dfPACKET_MOVE_DIR_LL;
    m_FlagField = 0;

    __super::Init();
}

void CPlayer::SetDirection(int _direction)
{
    m_direction = _direction;

    switch (m_direction)
    {
    case dfPACKET_MOVE_DIR_RR:
    case dfPACKET_MOVE_DIR_RU:
    case dfPACKET_MOVE_DIR_RD:
        m_facingDirection = dfPACKET_MOVE_DIR_RR;
        break;

    case dfPACKET_MOVE_DIR_LL:
    case dfPACKET_MOVE_DIR_LU:
    case dfPACKET_MOVE_DIR_LD:
        m_facingDirection = dfPACKET_MOVE_DIR_LL;
        break;

    default:
        break;
    }
}

void CPlayer::Damaged(int _hp)
{ 
    if (m_hp - _hp <= 0)
    {
        m_hp = 0;
        m_bDead = true;
    }
    else
        m_hp -= _hp;

    m_pSession->debugLogQueue.enqueue(std::make_tuple(
        m_curSectorPos.x,
        m_curSectorPos.y,
        m_direction,
        m_x,
        m_y,
        m_hp
    ));
}

void CPlayer::SetSpeed(UINT8 speedX, UINT8 speedY)
{
    m_speedX = speedX;
    m_speedY = speedY;
}

void CPlayer::SetFlag(UINT8 flag, bool bOnOff)
{
    if (bOnOff)
    {
        m_FlagField |= flag;
    }
    else
    {
        m_FlagField &= ~flag;
    }
}

void CPlayer::ToggleFlag(UINT8 flag)
{
    m_FlagField ^= flag;
}

bool CPlayer::isBitSet(UINT8 flag)
{
    return (m_FlagField & flag) != 0;
}

void CPlayer::SetFlagField(UINT8 pField)
{
    m_FlagField = pField;
}

void SendCreationPacketBetween(CObject* pObjectA, CObject* pObjectB)
{
    CPlayer* pPlayerA = static_cast<CPlayer*>(pObjectA);
    CPlayer* pPlayerB = static_cast<CPlayer*>(pObjectB);

    // A에게 B에 대한 정보를 생성하는 패킷을 전송
    SC_CREATE_OTHER_CHARACTER_FOR_SINGLE(pPlayerA->m_pSession, pPlayerB->m_ID, pPlayerB->m_facingDirection, pPlayerB->m_x, pPlayerB->m_y, pPlayerB->m_hp);

    // B가 움직이고 있다면
    if (pPlayerB->m_FlagField & FLAG_MOVING)
    {
        // A에게 B가 움직인다고 보냄
        SC_MOVE_START_FOR_SINGLE(pPlayerA->m_pSession, pPlayerB->m_ID, pPlayerB->m_direction, pPlayerB->m_x, pPlayerB->m_y);
    }

    // B에게 A에 대한 정보를 생성하는 패킷을 전송
    SC_CREATE_OTHER_CHARACTER_FOR_SINGLE(pPlayerB->m_pSession, pPlayerA->m_ID, pPlayerA->m_facingDirection, pPlayerA->m_x, pPlayerA->m_y, pPlayerA->m_hp);

    // A가 움직이고 있다면
    if (pPlayerA->m_FlagField & FLAG_MOVING)
    {
        // B에게 A가 움직인다고 보냄
        SC_MOVE_START_FOR_SINGLE(pPlayerB->m_pSession, pPlayerA->m_ID, pPlayerA->m_direction, pPlayerA->m_x, pPlayerA->m_y);
    }
}

void SendDestructionPacketBetween(CObject* pObjectA, CObject* pObjectB)
{
    CPlayer* pPlayerA = static_cast<CPlayer*>(pObjectA);
    CPlayer* pPlayerB = static_cast<CPlayer*>(pObjectB);

    // A에게 B에 대한 정보를 삭제하는 패킷을 전송
    SC_DELETE_CHARACTER_FOR_SINGLE(pPlayerA->m_pSession, pPlayerB->m_ID);

    // B에게 A에 대한 정보를 삭제하는 패킷을 전송
    SC_DELETE_CHARACTER_FOR_SINGLE(pPlayerB->m_pSession, pPlayerA->m_ID);
}