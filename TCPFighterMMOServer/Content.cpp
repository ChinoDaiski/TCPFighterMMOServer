
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

    // 1. ����� ���ǿ� PACKET_SC_CREATE_MY_CHARACTER �� ����
    // 2. PACKET_SC_CREATE_OTHER_CHARACTER �� ����� ������ ������ ��� ��ε�ĳ��Ʈ
    // 3. PACKET_SC_CREATE_OTHER_CHARACTER �� g_clientList�� �ִ� ��� ĳ���� ������ ��� ����� ���ǿ��� ����

    //=====================================================================================================================================
    // 1. ����� ���ǿ� PACKET_SC_CREATE_MY_CHARACTER �� ����
    //=====================================================================================================================================
    UINT16 posX, posY;
    pObj->getPosition(posX, posY);
    SC_CREATE_MY_CHARACTER_FOR_SINGLE(pSession, pPlayer->m_uID, pPlayer->GetDirection(), posX, posY, pPlayer->GetHp());

    //=====================================================================================================================================
    // 2. PACKET_SC_CREATE_OTHER_CHARACTER �� ����� ������ ������ ��� ��ε�ĳ��Ʈ
    //=====================================================================================================================================
    SC_CREATE_OTHER_CHARACTER_FOR_All(pSession, pPlayer->m_uID, pPlayer->GetDirection(), posX, posY, pPlayer->GetHp());

    //=====================================================================================================================================
    // 3. PACKET_SC_CREATE_OTHER_CHARACTER �� g_clientList�� �ִ� ��� ĳ���� ������ ��� ����� ���ǿ��� ����
    //=====================================================================================================================================

    // ���ο� ������ �õ��ϴ� Ŭ���̾�Ʈ�� ���� Ŭ���̾�Ʈ �������� ����
    CObjectManager& ObjectManager = CObjectManager::GetInstance();
    for (const auto& Object : ObjectManager.GetObjectList())
    {
        CPlayer* player = static_cast<CPlayer*>(Object);

        player->getPosition(posX, posY);
        SC_CREATE_OTHER_CHARACTER_FOR_SINGLE(pSession, player->m_uID, player->GetDirection(), posX, posY, player->GetHp());

        // �����̰� �ִ� ��Ȳ�̶��
        if (player->isBitSet(FLAG_MOVING))
        {
            SC_MOVE_START_FOR_SINGLE(pSession, player->m_uID, player->GetDirection(), posX, posY);
        }
    }
}