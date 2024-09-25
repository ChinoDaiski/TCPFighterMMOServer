
#include "pch.h"
#include "Content.h"
#include "Player.h"
#include "MakePacket.h"
#include "ObjectManager.h"
#include "SectorManager.h"
#include "Sector.h" 

CObject* CreateAcceptObject(void)
{
    // 오브젝트 생성 및 오브젝트를 섹터에 등록
    CObject* pObj = new CPlayer(
        rand() % (dfRANGE_MOVE_RIGHT - dfRANGE_MOVE_LEFT - 10) + dfRANGE_MOVE_LEFT + 10,  // rand() % 500 + 100,
        rand() % (dfRANGE_MOVE_BOTTOM - dfRANGE_MOVE_TOP - 10) + dfRANGE_MOVE_TOP + 10,   // rand() % 500 + 100,
        dfPACKET_MOVE_DIR_LL,
        100
    );

    CPlayer* pPlayer = static_cast<CPlayer*>(pObj);
    pPlayer->SetSpeed(dfSPEED_PLAYER_X, dfSPEED_PLAYER_Y);

	return pObj;
}

void LoginAcceptObject(CObject* pObj)
{
    CSession* pSession = pObj->m_pSession;
    CPlayer* pPlayer = static_cast<CPlayer*>(pObj);

    //=====================================================================================================================================
    // 1. 연결된 세션에 PACKET_SC_CREATE_MY_CHARACTER 를 전송
    //=====================================================================================================================================
    UINT16 posX, posY;
    pPlayer->getPosition(posX, posY);
    SC_CREATE_MY_CHARACTER_FOR_SINGLE(pSession, pPlayer->m_ID, pPlayer->GetDirection(), posX, posY, pPlayer->GetHp());

    //=====================================================================================================================================
    // 2. PACKET_SC_CREATE_OTHER_CHARACTER 에 새로 연결된 세션의 정보를 담아 주위 섹터들에게 send
    //=====================================================================================================================================

    static CSectorManager& sectorManager = CSectorManager::GetInstance();

    // 현재 오브젝트가 위치한 섹터정보 추출
    CSector* pCurSector = sectorManager.GetSectorInfo(pPlayer->m_curSectorPos.x, pPlayer->m_curSectorPos.y);

    // 현재 오브젝트 정보를 제외한 섹터들에 있는 오브젝트들에 새로 접속한 유저의 정보를 추가
    SC_CREATE_OTHER_CHARACTER_AROUND(pSession, pCurSector, pPlayer->m_ID, pPlayer->GetDirection(), posX, posY, pPlayer->GetHp());


    //=====================================================================================================================================
    // 3. PACKET_SC_CREATE_OTHER_CHARACTER 에 주위 섹터들에 있는 모든 캐릭터 정보들을 담아 새로 연결된 세션에게 전송
    //=====================================================================================================================================

    // 섹터 주위에 있는 섹터 리스트 추출
    auto& sectorList = pCurSector->GetAroundSectorList();

    // 새로운 연결을 시도하는 클라이언트에 섹터 주위에 있는 모든 오브젝트들의 정보 추가
    CPlayer* pSectorPlayer;
    for (auto& sector : sectorList)
    {
        for (auto& Object : sector->GetSectorObjectMap())
        {
            if (Object.second == pObj)
                continue;

            pSectorPlayer = static_cast<CPlayer*>(Object.second);
            pSectorPlayer->getPosition(posX, posY);

            SC_CREATE_OTHER_CHARACTER_FOR_SINGLE(pSession, pSectorPlayer->m_ID, pSectorPlayer->GetDirection(), posX, posY, pSectorPlayer->GetHp());

            // 움직이고 있는 상황이라면
            if (pSectorPlayer->isBitSet(FLAG_MOVING))
            {
                SC_MOVE_START_FOR_SINGLE(pSession, pSectorPlayer->m_ID, pSectorPlayer->GetDirection(), posX, posY);
            }
        }
    }
}