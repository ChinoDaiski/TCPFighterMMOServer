
#include "pch.h"
#include "Sector.h"
#include "RingBuffer.h"
#include "Object.h"

#define dfPEAK_TO_AVERAGE_CAPACITY_RATIO	5		// �� ���Ϳ��� �ٸ� ���� ��� ��� �ִ� ���� ���� ���

CSector::CSector(UINT16 posX, UINT16 posY) noexcept
	: m_posX(posX), m_posY(posY)
{
	// 9�� ���ϴ°� �ڽ��� ������ �ִ� 9���� ���� ���Ϳ��� ��Ŷ�� ������ ������ �� �ֱ� ����
	UINT32 ringBufferSize = dfEXPECTED_ACTIVE_USERS / (dfSECTOR_WIDTH_CNT * dfSECTOR_HEIGHT_CNT) * (dfMAX_PACKET_SIZE + sizeof(PACKET_HEADER)) * 9 * dfPEAK_TO_AVERAGE_CAPACITY_RATIO;

	m_sectorObjectMap.clear();
}

CSector::~CSector(void)
{
}

void CSector::RegisterObject(CObject* pObject)
{
	m_sectorObjectMap.emplace(pObject->m_ID, pObject);
}

void CSector::DeleteObject(CObject* pObject)
{
#ifdef _DEBUG
	auto iter = m_sectorObjectMap.find(pObject->m_ID);

	if (iter != m_sectorObjectMap.end())
	{
		DebugBreak();
	}
#endif // _DEBUG

	m_sectorObjectMap.erase(pObject->m_ID);
}
