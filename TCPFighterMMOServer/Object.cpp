
#include "pch.h"
#include "Object.h"
#include "Session.h"

CObject::CObject(UINT16 _x, UINT16 _y) noexcept
	: m_x(_x), m_y(_y), m_pSession(nullptr), m_bDead(false)
{
}

void CObject::Move()
{
}

void CObject::Update(void)
{
}

void CObject::LateUpdate(void)
{
}
