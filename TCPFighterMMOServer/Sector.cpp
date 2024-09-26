
#include "pch.h"
#include "Sector.h"
#include "RingBuffer.h"
#include "Object.h"

#define dfPEAK_TO_AVERAGE_CAPACITY_RATIO	5		// 한 섹터에서 다른 섹터 평균 대비 최대 수용 가능 배수

CSector::CSector(UINT8 posX, UINT8 posY) noexcept
	: m_posX(posX), m_posY(posY)
{
	// 9를 곱하는건 자신을 포함한 최대 9개의 주위 섹터에서 패킷에 정보를 적재할 수 있기 때문
	UINT32 ringBufferSize = dfEXPECTED_ACTIVE_USERS / (dfSECTOR_WIDTH_CNT * dfSECTOR_HEIGHT_CNT) * (dfMAX_PACKET_SIZE + sizeof(PACKET_HEADER)) * 9 * dfPEAK_TO_AVERAGE_CAPACITY_RATIO;

	m_sectorObjectMap.clear();
}

CSector::~CSector(void)
{
	m_aroundSectorlist.clear();
	m_sectorObjectMap.clear();
}

void CSector::RegisterObject(CObject* pObject)
{
	m_sectorObjectMap.emplace(pObject->m_ID, pObject);
}

void CSector::DeleteObject(CObject* pObject)
{
	auto iter = m_sectorObjectMap.find(pObject->m_ID);

#ifdef _DEBUG
	if (iter == m_sectorObjectMap.end())
	{
		DebugBreak();
	}
#endif // _DEBUG

	m_sectorObjectMap.erase(iter);
}
