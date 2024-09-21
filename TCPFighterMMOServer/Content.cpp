
#include "pch.h"
#include "Content.h"
#include "Player.h"
#include "MakePacket.h"
#include "ObjectManager.h"

CObject* CreateAcceptObject(void)
{
	CObject* pObj = new CPlayer(
		rand() % (dfRANGE_MOVE_RIGHT - dfRANGE_MOVE_LEFT) + dfRANGE_MOVE_LEFT,
		rand() % (dfRANGE_MOVE_BOTTOM - dfRANGE_MOVE_TOP) + dfRANGE_MOVE_TOP,
		dfPACKET_MOVE_DIR_LL,
		100
	);

	return pObj;
}

void LoginAcceptObject(CObject* pObj)
{
    CSession* pSession = pObj->m_pSession;
    CPlayer* pPlayer = static_cast<CPlayer*>(pObj);

    // 1. 연결된 세션에 PACKET_SC_CREATE_MY_CHARACTER 를 전송
    // 2. PACKET_SC_CREATE_OTHER_CHARACTER 에 연결된 세션의 정보를 담아 브로드캐스트
    // 3. PACKET_SC_CREATE_OTHER_CHARACTER 에 g_clientList에 있는 모든 캐릭터 정보를 담아 연결된 세션에게 전송

    //=====================================================================================================================================
    // 1. 연결된 세션에 PACKET_SC_CREATE_MY_CHARACTER 를 전송
    //=====================================================================================================================================
    UINT16 posX, posY;
    pObj->getPosition(posX, posY);
    SC_CREATE_MY_CHARACTER_FOR_SINGLE(pSession, pPlayer->m_uID, pPlayer->GetDirection(), posX, posY, pPlayer->GetHp());

    //=====================================================================================================================================
    // 2. PACKET_SC_CREATE_OTHER_CHARACTER 에 연결된 세션의 정보를 담아 브로드캐스트
    //=====================================================================================================================================
    SC_CREATE_OTHER_CHARACTER_FOR_All(pSession, pPlayer->m_uID, pPlayer->GetDirection(), posX, posY, pPlayer->GetHp());

    //=====================================================================================================================================
    // 3. PACKET_SC_CREATE_OTHER_CHARACTER 에 g_clientList에 있는 모든 캐릭터 정보를 담아 연결된 세션에게 전송
    //=====================================================================================================================================

    // 새로운 연결을 시도하는 클라이언트에 기존 클라이언트 정보들을 전달
    CObjectManager& ObjectManager = CObjectManager::GetInstance();
    for (const auto& Object : ObjectManager.GetObjectList())
    {
        CPlayer* player = static_cast<CPlayer*>(Object);

        player->getPosition(posX, posY);
        SC_CREATE_OTHER_CHARACTER_FOR_SINGLE(pSession, player->m_uID, player->GetDirection(), posX, posY, player->GetHp());

        // 움직이고 있는 상황이라면
        if (player->isBitSet(FLAG_MOVING))
        {
            SC_MOVE_START_FOR_SINGLE(pSession, player->m_uID, player->GetDirection(), posX, posY);
        }
    }
}