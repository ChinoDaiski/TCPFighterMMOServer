
#include "pch.h"
#include "Player.h"

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
    if (!m_FlagField)
        return;

    if (m_FlagField & FLAG_MOVING)
        Move();
}

void CPlayer::LateUpdate(void)
{
}

void CPlayer::Move(void)
{
    // �̵��ϴ� ��ġ ���
    int resultX = m_x + (dir[m_direction][0] * m_speedX);
    int resultY = m_y + (dir[m_direction][1] * m_speedY);

    // ���� �˻�
    if (
        (resultX > dfRANGE_MOVE_RIGHT) ||
        (resultX < dfRANGE_MOVE_LEFT) ||
        (resultY > dfRANGE_MOVE_BOTTOM) ||
        (resultY < dfRANGE_MOVE_TOP)
        )
        return;
    // ���� �˻� �����
    else
    {
        // ��ġ ����
        m_x = resultX;
        m_y = resultY;
    }
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
