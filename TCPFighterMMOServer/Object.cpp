
#pragma once

#include "pch.h"
#include "Object.h"
#include "Session.h"
#include "SectorManager.h"

UINT32 CObject::g_ID = 0;

CObject::CObject(UINT16 _x, UINT16 _y) noexcept
	: m_x(_x), m_y(_y), m_pSession(nullptr), m_bDead(false)
{
	m_ID = g_ID;
	g_ID++;

	// 플레이어가 생성될 시 섹터 매니저에 등록
	static CSectorManager& sectorManager = CSectorManager::GetInstance();
	sectorManager.RegisterObjectToSector(this);
}

void CObject::Move()
{
}

void CObject::Update(void)
{
}

void CObject::LateUpdate(void)
{
	std::pair<UINT8, UINT8> curIndex = CSectorManager::GetSectorIndexFromWorldCoords(m_x, m_y);

	// 이전 위치 백업
	m_preSectorPos.x = m_curSectorPos.x;
	m_preSectorPos.y = m_curSectorPos.y;

	// 현재 위치 최신화
	m_curSectorPos.x = curIndex.first;
	m_curSectorPos.y = curIndex.second;

	if ((m_preSectorPos.x != m_curSectorPos.x) || (m_preSectorPos.y != m_curSectorPos.y))
	{
		static CSectorManager& sectorManager = CSectorManager::GetInstance();
		sectorManager.CalculateSectorChanges(this);
	}
}
