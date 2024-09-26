
#pragma once

#include "pch.h"
#include "Object.h"
#include "Session.h"
#include "SectorManager.h"
#include "TimerManager.h"

UINT32 CObject::g_ID = 0;
static CTimerManager& timerManager = CTimerManager::GetInstance();

CObject::CObject(UINT16 _x, UINT16 _y) noexcept
	: m_x(_x), m_y(_y), m_pSession(nullptr), m_bDead(false)
{
	m_ID = g_ID;
	g_ID++;

	// �÷��̾ ������ �� ���� �Ŵ����� ���
	static CSectorManager& sectorManager = CSectorManager::GetInstance();
	sectorManager.RegisterObjectToSector(this);
}

void CObject::Move()
{
}

void CObject::Update(void)
{
	CheckTimeout();
}

void CObject::LateUpdate(void)
{
	std::pair<UINT8, UINT8> curIndex = CSectorManager::GetSectorIndexFromWorldCoords(m_x, m_y);

	// ���� ��ġ ���
	m_preSectorPos.x = m_curSectorPos.x;
	m_preSectorPos.y = m_curSectorPos.y;

	// ���� ��ġ �ֽ�ȭ
	m_curSectorPos.x = curIndex.first;
	m_curSectorPos.y = curIndex.second;

	if ((m_preSectorPos.x != m_curSectorPos.x) || (m_preSectorPos.y != m_curSectorPos.y))
	{
		static CSectorManager& sectorManager = CSectorManager::GetInstance();
		sectorManager.CalculateSectorChanges(this);
	}
}

void CObject::CheckTimeout(void)
{
	UINT32 currSeverTime = timerManager.GetCurrServerTime();
	if ((currSeverTime - m_lastTimeoutCheckTime) > dfNETWORK_PACKET_RECV_TIMEOUT)
	{
		m_bDead = true;
	}
	else
		m_lastTimeoutCheckTime = currSeverTime;
}

void CObject::SetCurTimeout(void)
{
	m_lastTimeoutCheckTime = timerManager.GetCurrServerTime();


}
