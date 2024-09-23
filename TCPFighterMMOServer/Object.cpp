
#include "pch.h"
#include "Object.h"
#include "Session.h"

UINT32 CObject::g_ID = 0;

CObject::CObject(UINT16 _x, UINT16 _y) noexcept
	: m_x(_x), m_y(_y), m_pSession(nullptr), m_bDead(false)
{
	m_ID = g_ID;
	g_ID++;
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
