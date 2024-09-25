
#include "pch.h"

#include "SectorManager.h"
#include "Sector.h"

#include "ObjectManager.h"
#include "Object.h"

#include "SessionManager.h"

UINT16 CSectorManager::sectorWidth = 0;
UINT16 CSectorManager::sectorHeight = 0;

void (*CSectorManager::m_callbackCreateSector)(CObject*, CObject*) = nullptr; // 초기화
void (*CSectorManager::m_callbackDeleteSector)(CObject*, CObject*) = nullptr; // 초기화

int direction[8][2] = {
    {-1, 0},	// LL
    {-1, -1},	// LU
    {0, -1},	// UU
    {1, -1},	// RU
    {1, 0},		// RR
    {1, 1},		// RD
    {0, 1},		// DD
    {-1, 1}	    // LD
};

CSectorManager::CSectorManager() noexcept
{
	for (UINT8 i = 0; i < dfSECTOR_HEIGHT_CNT; ++i)
	{
		for (UINT8 j = 0; j < dfSECTOR_WIDTH_CNT; ++j)
		{
			m_Sectors[i][j] = new CSector(i, j);
		}
	}

    UINT8 posX, posY;
    for (UINT8 i = 0; i < dfSECTOR_HEIGHT_CNT; ++i)
    {
        for (UINT8 j = 0; j < dfSECTOR_WIDTH_CNT; ++j)
        {
            for (UINT8 k = 0; k < 8; ++k)
            {
                posX = j + direction[k][0];
                posY = i + direction[k][1];

                if (
                    (0 <= posX && posX < dfSECTOR_WIDTH_CNT) &&
                    (0 <= posY && posY < dfSECTOR_HEIGHT_CNT)
                    )
                {
                    m_Sectors[i][j]->InsertAroundSector(m_Sectors[posY][posX]);
                }
            }

            m_Sectors[i][j]->InsertAroundSector(m_Sectors[i][j]);
        }
    }

    // 섹터에 들어가 있는 주위 섹터 갯수 (자기 포함)을 확인
    /*
    for (UINT8 i = 0; i < dfSECTOR_HEIGHT_CNT; ++i)
    {
        for (UINT8 j = 0; j < dfSECTOR_WIDTH_CNT; ++j)
        {
            std::cout << m_Sectors[i][j]->GetAroundSectorList().size();
        }
        std::cout << "\n";
    }
    */

	sectorWidth = (dfRANGE_MOVE_RIGHT - dfRANGE_MOVE_LEFT) / dfSECTOR_WIDTH_CNT;
	sectorHeight = (dfRANGE_MOVE_BOTTOM - dfRANGE_MOVE_TOP) / dfSECTOR_HEIGHT_CNT;

	m_sectorsToDelete.reserve(5);
	m_sectorsToAdd.reserve(5);
}

CSectorManager::~CSectorManager() noexcept
{
	for (UINT8 i = 0; i < dfSECTOR_HEIGHT_CNT; ++i)
	{
		for (UINT8 j = 0; j < dfSECTOR_WIDTH_CNT; ++j)
		{
			delete m_Sectors[i][j];
		}
	}
}

std::pair<UINT8, UINT8> CSectorManager::GetSectorIndexFromWorldCoords(UINT16 prePosX, UINT16 prePosY)
{
	return std::make_pair(prePosX / sectorWidth, prePosY / sectorHeight);
}

void CSectorManager::CalculateSectorChanges(CObject* pObject)
{
	// 오브젝트의 위치가 변경되었다면
	if ((pObject->m_preSectorPos.x != pObject->m_curSectorPos.x) || (pObject->m_preSectorPos.y != pObject->m_curSectorPos.y))
	{
        m_sectorsToAdd.clear();
        m_sectorsToDelete.clear();

        int moveDirX = pObject->m_curSectorPos.x - pObject->m_preSectorPos.x;
        int moveDirY = pObject->m_curSectorPos.y - pObject->m_preSectorPos.y;

        // 65536... 가로 세로 월드 위치 범위가 이를 넘어가진 않겠지?
        UINT16 x = pObject->m_preSectorPos.x;
        UINT16 y = pObject->m_preSectorPos.y;

        // 대각선 이동
        if (moveDirX != 0 && moveDirY != 0) { // 대각선 이동
            if (moveDirX == -1 && moveDirY == -1) { // 좌상
                m_sectorsToDelete.push_back(POINT{ x + 1, y + 1 }); // 우하단
                m_sectorsToDelete.push_back(POINT{ x + 1, y });     // 우측
                m_sectorsToDelete.push_back(POINT{ x + 1, y - 1 }); // 우상단
                m_sectorsToDelete.push_back(POINT{ x, y + 1 });     // 하단
                m_sectorsToDelete.push_back(POINT{ x - 1, y + 1 }); // 좌하단

                x = pObject->m_curSectorPos.x;
                y = pObject->m_curSectorPos.y;

                m_sectorsToAdd.push_back(POINT{ x - 1, y });     // 좌측
                m_sectorsToAdd.push_back(POINT{ x - 1, y + 1 }); // 좌하단
                m_sectorsToAdd.push_back(POINT{ x - 1, y - 1 }); // 좌상단
                m_sectorsToAdd.push_back(POINT{ x, y - 1 });     // 상단
                m_sectorsToAdd.push_back(POINT{ x + 1, y - 1 }); // 우상단
            }
            else if (moveDirX == -1 && moveDirY == 1) { // 좌하
                m_sectorsToDelete.push_back(POINT{ x + 1, y - 1 }); // 우상단
                m_sectorsToDelete.push_back(POINT{ x + 1, y });     // 우측
                m_sectorsToDelete.push_back(POINT{ x + 1, y + 1 }); // 우하단
                m_sectorsToDelete.push_back(POINT{ x, y - 1 });     // 상단
                m_sectorsToDelete.push_back(POINT{ x - 1, y - 1 }); // 좌상단

                x = pObject->m_curSectorPos.x;
                y = pObject->m_curSectorPos.y;

                m_sectorsToAdd.push_back(POINT{ x - 1, y });     // 좌측
                m_sectorsToAdd.push_back(POINT{ x - 1, y - 1 }); // 좌상단
                m_sectorsToAdd.push_back(POINT{ x - 1, y + 1 }); // 좌하단
                m_sectorsToAdd.push_back(POINT{ x, y + 1 });     // 하단
                m_sectorsToAdd.push_back(POINT{ x + 1, y + 1 }); // 우하단
            }
            else if (moveDirX == 1 && moveDirY == -1) { // 우상
                m_sectorsToDelete.push_back(POINT{ x - 1, y + 1 }); // 좌하단
                m_sectorsToDelete.push_back(POINT{ x - 1, y });     // 좌측
                m_sectorsToDelete.push_back(POINT{ x - 1, y - 1 }); // 좌상단
                m_sectorsToDelete.push_back(POINT{ x, y + 1 });     // 하단
                m_sectorsToDelete.push_back(POINT{ x + 1, y + 1 }); // 우하단

                x = pObject->m_curSectorPos.x;
                y = pObject->m_curSectorPos.y;

                m_sectorsToAdd.push_back(POINT{ x + 1, y });     // 우측
                m_sectorsToAdd.push_back(POINT{ x + 1, y - 1 }); // 우상단
                m_sectorsToAdd.push_back(POINT{ x + 1, y + 1 }); // 우하단
                m_sectorsToAdd.push_back(POINT{ x, y - 1 });     // 상단
                m_sectorsToAdd.push_back(POINT{ x - 1, y - 1 }); // 좌상단
            }
            else if (moveDirX == 1 && moveDirY == 1) { // 우하
                m_sectorsToDelete.push_back(POINT{ x - 1, y - 1 }); // 좌상단
                m_sectorsToDelete.push_back(POINT{ x - 1, y });     // 좌측
                m_sectorsToDelete.push_back(POINT{ x - 1, y + 1 }); // 좌하단
                m_sectorsToDelete.push_back(POINT{ x, y - 1 });     // 상단
                m_sectorsToDelete.push_back(POINT{ x + 1, y - 1 }); // 우상단

                x = pObject->m_curSectorPos.x;
                y = pObject->m_curSectorPos.y;

                m_sectorsToAdd.push_back(POINT{ x + 1, y });     // 우측
                m_sectorsToAdd.push_back(POINT{ x + 1, y + 1 }); // 우하단
                m_sectorsToAdd.push_back(POINT{ x, y + 1 });     // 하단
                m_sectorsToAdd.push_back(POINT{ x - 1, y + 1 }); // 좌하단
                m_sectorsToAdd.push_back(POINT{ x + 1, y - 1 }); // 우상단
            }
        }
        // 상하좌우 이동
        else if (moveDirX != 0 || moveDirY != 0) {
            if (moveDirX == -1 && moveDirY == 0) { // 좌
                m_sectorsToDelete.push_back(POINT{ x + 1, y - 1 }); // 우상단
                m_sectorsToDelete.push_back(POINT{ x + 1, y });     // 우측
                m_sectorsToDelete.push_back(POINT{ x + 1, y + 1 }); // 우하단

                x = pObject->m_curSectorPos.x;
                y = pObject->m_curSectorPos.y;

                m_sectorsToAdd.push_back(POINT{ x - 1, y - 1 }); // 좌상단
                m_sectorsToAdd.push_back(POINT{ x - 1, y });     // 좌측
                m_sectorsToAdd.push_back(POINT{ x - 1, y + 1 }); // 좌하단
            }
            else if (moveDirX == 1 && moveDirY == 0) { // 우
                m_sectorsToDelete.push_back(POINT{ x - 1, y - 1 }); // 좌상단
                m_sectorsToDelete.push_back(POINT{ x - 1, y });     // 좌측
                m_sectorsToDelete.push_back(POINT{ x - 1, y + 1 }); // 좌하단

                x = pObject->m_curSectorPos.x;
                y = pObject->m_curSectorPos.y;

                m_sectorsToAdd.push_back(POINT{ x + 1, y - 1 }); // 우상단
                m_sectorsToAdd.push_back(POINT{ x + 1, y });     // 우측
                m_sectorsToAdd.push_back(POINT{ x + 1, y + 1 }); // 우하단
            }
            else if (moveDirX == 0 && moveDirY == -1) { // 위
                m_sectorsToDelete.push_back(POINT{ x - 1, y + 1 }); // 좌하단
                m_sectorsToDelete.push_back(POINT{ x, y + 1 });     // 하단
                m_sectorsToDelete.push_back(POINT{ x + 1, y + 1 }); // 우하단

                x = pObject->m_curSectorPos.x;
                y = pObject->m_curSectorPos.y;

                m_sectorsToAdd.push_back(POINT{ x - 1, y - 1 }); // 좌상단
                m_sectorsToAdd.push_back(POINT{ x, y - 1 });     // 상단
                m_sectorsToAdd.push_back(POINT{ x + 1, y - 1 }); // 우상단
            }
            else if (moveDirX == 0 && moveDirY == 1) { // 아래
                m_sectorsToDelete.push_back(POINT{ x - 1, y - 1 }); // 좌상단
                m_sectorsToDelete.push_back(POINT{ x, y - 1 });     // 상단
                m_sectorsToDelete.push_back(POINT{ x + 1, y - 1 }); // 우상단

                x = pObject->m_curSectorPos.x;
                y = pObject->m_curSectorPos.y;

                m_sectorsToAdd.push_back(POINT{ x - 1, y + 1 }); // 좌하단
                m_sectorsToAdd.push_back(POINT{ x, y + 1 });     // 하단
                m_sectorsToAdd.push_back(POINT{ x + 1, y + 1 }); // 우하단
            }
        }

        // 그리드 경계를 벗어나지 않도록 필터링
        m_sectorsToDelete.erase(std::remove_if(m_sectorsToDelete.begin(), m_sectorsToDelete.end(),
            [](const POINT& pos) {
                return pos.x < 0 || pos.x >= dfSECTOR_WIDTH_CNT || pos.y < 0 || pos.y >= dfSECTOR_HEIGHT_CNT;
            }), m_sectorsToDelete.end());

        m_sectorsToAdd.erase(std::remove_if(m_sectorsToAdd.begin(), m_sectorsToAdd.end(),
            [](const POINT& pos) {
                return pos.x < 0 || pos.x >= dfSECTOR_WIDTH_CNT || pos.y < 0 || pos.y >= dfSECTOR_HEIGHT_CNT;
            }), m_sectorsToAdd.end());

        // 오브젝트 생성 및 삭제 처리
        ProcessSectorObjectPackets(pObject);
	}
}

void CSectorManager::ProcessSectorObjectPackets(CObject* pMovingObject)
{
    CObject* pSectorObject;

    // 소멸되어야하는 섹터들이 가지고 있는 오브젝트들에게 내 오브젝트 정보 삭제 패킷을 보내고, 내 오브젝트에게 해당 섹터들이 가지고 있는 오브젝트들 삭제 패킷을 주입
    for (auto& sector : m_sectorsToDelete)
    {
        for (auto& mapIndex : m_Sectors[sector.y][sector.x]->GetSectorObjectMap())
        {
            pSectorObject = mapIndex.second;

            // 서로에게 삭제 패킷 정보를 보냄.
            m_callbackDeleteSector(pMovingObject, pSectorObject);
        }
    }
    m_Sectors[pMovingObject->m_preSectorPos.y][pMovingObject->m_preSectorPos.x]->DeleteObject(pMovingObject);

    // 생성되어야하는 섹터들이 가지고 있는 오브젝트들에게 내 오브젝트 정보 생성 패킷을 보내고, 내 오브젝트에게 해당 섹터들이 가지고 있는 오브젝트들 생성 패킷을 주입
    // 생성 패킷을 보낼 때 내 오브젝트의 상태도 함께 보낸다. 마찬가지로 생성이 되어야하는 오브젝트들 정보를 받을 때 해당 오브젝트들의 상태로 주입 받음.
    for (auto& sector : m_sectorsToAdd)
    {
        for (auto& mapIndex : m_Sectors[sector.y][sector.x]->GetSectorObjectMap())
        {
            pSectorObject = mapIndex.second;

            // 서로에게 생성관련 패킷 정보를 보냄.
            m_callbackCreateSector(pMovingObject, pSectorObject);
        }
    }
    m_Sectors[pMovingObject->m_curSectorPos.y][pMovingObject->m_curSectorPos.x]->RegisterObject(pMovingObject);
}

void CSectorManager::RegisterObjectToSector(CObject* pObject)
{
    // 현재 오브젝트의 정보를 받아 섹터에 등록
    UINT16 posX, posY;
    pObject->getPosition(posX, posY);
    std::pair<UINT8, UINT8> curIndex = CSectorManager::GetSectorIndexFromWorldCoords(posX, posY);
    pObject->m_curSectorPos.x = curIndex.first;
    pObject->m_curSectorPos.y = curIndex.second;

    m_Sectors[pObject->m_curSectorPos.y][pObject->m_curSectorPos.x]->RegisterObject(pObject);
}

void CSectorManager::DeleteObjectFromSector(CObject* pObject)
{
    // 현재 오브젝트의 정보를 받아 섹터에서 삭제
    UINT16 posX, posY;
    m_Sectors[pObject->m_curSectorPos.y][pObject->m_curSectorPos.x]->DeleteObject(pObject);
}
