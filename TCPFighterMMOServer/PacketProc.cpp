#include "pch.h"
#include "PacketProc.h"
#include "Packet.h"
#include "MakePacket.h"

#include "SessionManager.h"

#include "Player.h"

bool PacketProc(CSession* pSession, PACKET_TYPE packetType, CPacket* pPacket)
{
    switch (packetType)
    {
        /*case PACKET_TYPE::CS_MOVE_START:
    {
        UINT8 Direction;
        UINT16 X;
        UINT16 Y;

        *pPacket >> Direction;
        *pPacket >> X;
        *pPacket >> Y;

        return CS_MOVE_START(pSession, Direction, X, Y);
    }
    break;
    case PACKET_TYPE::CS_MOVE_STOP:
    {
        UINT8 Direction;
        UINT16 X;
        UINT16 Y;

        *pPacket >> Direction;
        *pPacket >> X;
        *pPacket >> Y;

        return CS_MOVE_STOP(pSession, Direction, X, Y);
    }
    break;
    case PACKET_TYPE::CS_ATTACK1:
    {
        UINT8 Direction;
        UINT16 X;
        UINT16 Y;

        *pPacket >> Direction;
        *pPacket >> X;
        *pPacket >> Y;

        return CS_ATTACK1(pSession, Direction, X, Y);
    }
    break;
    case PACKET_TYPE::CS_ATTACK2:
    {
        UINT8 Direction;
        UINT16 X;
        UINT16 Y;

        *pPacket >> Direction;
        *pPacket >> X;
        *pPacket >> Y;

        return CS_ATTACK2(pSession, Direction, X, Y);
    }
    break;
    case PACKET_TYPE::CS_ATTACK3:
    {
        UINT8 Direction;
        UINT16 X;
        UINT16 Y;

        *pPacket >> Direction;
        *pPacket >> X;
        *pPacket >> Y;

        return CS_ATTACK3(pSession, Direction, X, Y);
    }
    break;*/
    default:
        return false;
        break;
    }
    return false;
}


//bool CS_MOVE_START(CSession* pSession, UINT8 direction, UINT16 x, UINT16 y)
//{
//    // �޽��� ���� �α� Ȯ��
//    // ==========================================================================================================
//    // ������ ��ġ�� ���� ��Ŷ�� ��ġ���� �ʹ� ū ���̰� ���ٸ� �������                           .
//    // �� ������ ��ǥ ����ȭ ������ �ܼ��� Ű���� ���� (Ŭ����Ʈ�� ��ó��, ������ �� �ݿ�) �������
//    // Ŭ���̾�Ʈ�� ��ǥ�� �״�� �ϴ� ����� ���ϰ� ����.
//    // ���� �¶��� �����̶�� Ŭ���̾�Ʈ���� �������� �����ϴ� ����� ���ؾ���.
//    // ������ ������ ������ �������� �ϰ� �����Ƿ� �������� ������ Ŭ���̾�Ʈ ��ǥ�� �ϵ��� �Ѵ�.
//    // ==========================================================================================================
//
//    CPlayer* pPlayer = static_cast<CPlayer*>(pSession->pObj);
//
//    UINT16 posX, posY;
//    pPlayer->getPosition(posX, posY);
//    if (
//        std::abs(posX - x) > dfERROR_RANGE ||
//        std::abs(posY - y) > dfERROR_RANGE
//        )
//    {
//        NotifyClientDisconnected(pSession);
//
//        // �α� �����Ÿ� ���⼭ ���� ��
//        int gapX = std::abs(posX - x);
//        int gapY = std::abs(posY - y);
//        DebugBreak();
//
//        return false;
//    }
//
//    // ==========================================================================================================
//    // ������ ����. ���� �������� ���۹�ȣ�� ���Ⱚ. ���ο��� �ٶ󺸴� ���⵵ ����
//    // ==========================================================================================================
//    pPlayer->SetDirection(direction);
//
//
//    // ==========================================================================================================
//    // ����ڸ� ������, ���� �������� ��� ����ڿ��� ��Ŷ�� �Ѹ�.
//    // ==========================================================================================================
//    SC_MOVE_START_FOR_All(pSession, pSession->uid, pPlayer->GetDirection(), x, y);
//
//
//    //=====================================================================================================================================
//    // �̵� ���� ������ �˸�
//    //=====================================================================================================================================
//    pPlayer->SetFlag(FLAG_MOVING, true);
//
//    return true;
//}
//
//bool CS_MOVE_STOP(CSession* pSession, UINT8 direction, UINT16 x, UINT16 y)
//{
//    // ���� ���õ� Ŭ���̾�Ʈ�� �������� �������� ���� ���̶� ��û
//    // 1. ���� ������ ó��
//    // 2. PACKET_SC_MOVE_STOP �� ��ε�ĳ����
//    // 3. ���� ������ �̵� ���� ������ �˸�
//
//    CPlayer* pPlayer = static_cast<CPlayer*>(pSession->pObj);
//
//    //=====================================================================================================================================
//    // 1. ���� ������ ó��
//    //=====================================================================================================================================
//    pPlayer->SetDirection(direction);
//    pPlayer->SetPosition(x, y);
//
//    //=====================================================================================================================================
//    // 2. PACKET_SC_MOVE_STOP �� ��ε�ĳ����
//    //=====================================================================================================================================
//    SC_MOVE_STOP_FOR_All(pSession, pSession->uid, pPlayer->GetDirection(), x, y);
//
//    //=====================================================================================================================================
//    // 3. ���� ������ �̵� ���� ������ �˸�
//    //=====================================================================================================================================
//    pPlayer->SetFlag(FLAG_MOVING, false);
//
//    return true;
//}
//
//bool CS_ATTACK1(CSession* pSession, UINT8 direction, UINT16 x, UINT16 y)
//{
//    // Ŭ���̾�Ʈ�� ���� ���� �޽����� ����.
//    // g_clientList�� ��ȸ�ϸ� ���� 1�� ������ �����ؼ� �������� �־���.
//    // 1. dfPACKET_SC_ATTACK1 �� ��ε�ĳ����
//    // 2. ���ݹ��� ĳ���͸� �˻�. �˻��� �����ϸ� 3, 4�� ���� ����
//    // 3. dfPACKET_SC_DAMAGE �� ��ε�ĳ����
//    // 4. ���� ü���� 0 ���Ϸ� �������ٸ� dfPACKET_SC_DELETE_CHARACTER �� ��ε�ĳ�����ϰ�, �������� ������ �� �ֵ��� �� -> �� �κ��� �������� ó���ϵ��� �ٲ�.
//
//    CPlayer* pPlayer = static_cast<CPlayer*>(pSession->pObj);
//
//    //=====================================================================================================================================
//    // 1. dfPACKET_SC_ATTACK1 �� ��ε�ĳ����
//    //=====================================================================================================================================
//    pPlayer->SetPosition(x, y);
//
//    SC_ATTACK1_FOR_All(pSession, pSession->uid, pPlayer->GetDirection(), x, y);
//
//    //=====================================================================================================================================
//    // 2. ���ݹ��� ĳ���͸� �˻�. �˻��� �����ϸ� 3, 4�� ���� ����
//    //=====================================================================================================================================
//
//    // ���� �ٶ󺸴� ���⿡ ���� ���� ������ �޶���.
//    UINT16 left, right, top, bottom;
//    UINT16 posX, posY;
//    pPlayer->getPosition(posX, posY);
//
//    // ������ �ٶ󺸰� �־��ٸ�
//    if (pPlayer->GetFacingDirection() == dfPACKET_MOVE_DIR_LL)
//    {
//        left = posX - dfATTACK1_RANGE_X;
//        right = posX;
//    }
//    // �������� �ٶ󺸰� �־��ٸ�
//    else
//    {
//        left = posX;
//        right = posX + dfATTACK1_RANGE_X;
//    }
//
//    top = posY - dfATTACK1_RANGE_Y;
//    bottom = posY + dfATTACK1_RANGE_Y;
//
//    for (auto& client : g_clientList)
//    {
//        if (client->uid == pSession->uid)
//            continue;
//
//        pPlayer->getPosition(posX, posY);
//
//        // �ٸ� �÷��̾��� ��ǥ�� ���� ������ ���� ���
//        if (posX >= left && posX <= right &&
//            posY >= top && posY <= bottom)
//        {
//            //=====================================================================================================================================
//            // 3. dfPACKET_SC_DAMAGE �� ��ε�ĳ����
//            //=====================================================================================================================================
//            // 1�� �������� �Ե��� ��
//            pPlayer->Damaged(dfATTACK1_DAMAGE);
//
//            SC_DAMAGE_FOR_All(nullptr, pSession->uid, client->uid, pPlayer->GetHp());
//            break;
//        }
//    }
//
//    return true;
//}
//
//bool CS_ATTACK2(CSession* pSession, UINT8 direction, UINT16 x, UINT16 y)
//{
//    // Ŭ���̾�Ʈ�� ���� ���� �޽����� ����.
//    // g_clientList�� ��ȸ�ϸ� ���� 2�� ������ �����ؼ� �������� �־���.
//    // 1. dfPACKET_SC_ATTACK2 �� ��ε�ĳ����
//    // 2. ���ݹ��� ĳ���͸� �˻�. �˻��� �����ϸ� 3, 4�� ���� ����
//    // 3. dfPACKET_SC_DAMAGE �� ��ε�ĳ����
//    // 4. ���� ü���� 0 ���Ϸ� �������ٸ� dfPACKET_SC_DELETE_CHARACTER �� ��ε�ĳ�����ϰ�, �������� ������ �� �ֵ��� ��
//
//    CPlayer* pPlayer = static_cast<CPlayer*>(pSession->pObj);
//
//    //=====================================================================================================================================
//    // 1. dfPACKET_SC_ATTACK2 �� ��ε�ĳ����
//    //=====================================================================================================================================
//    pPlayer->SetPosition(x, y);
//
//    SC_ATTACK2_FOR_All(pSession, pSession->uid, pPlayer->GetDirection(), x, y);
//
//    //=====================================================================================================================================
//    // 2. ���ݹ��� ĳ���͸� �˻�. �˻��� �����ϸ� 3, 4�� ���� ����
//    //=====================================================================================================================================
//
//    // ���� �ٶ󺸴� ���⿡ ���� ���� ������ �޶���.
//    UINT16 left, right, top, bottom;
//    UINT16 posX, posY;
//    pPlayer->getPosition(posX, posY);
//
//    // ������ �ٶ󺸰� �־��ٸ�
//    if (pPlayer->GetFacingDirection() == dfPACKET_MOVE_DIR_LL)
//    {
//        left = posX - dfATTACK2_RANGE_X;
//        right = posX;
//    }
//    // �������� �ٶ󺸰� �־��ٸ�
//    else
//    {
//        left = posX;
//        right = posX + dfATTACK2_RANGE_X;
//    }
//
//    top = posY - dfATTACK2_RANGE_Y;
//    bottom = posY + dfATTACK2_RANGE_Y;
//
//    for (auto& client : g_clientList)
//    {
//        if (client->uid == pSession->uid)
//            continue;
//
//        pPlayer->getPosition(posX, posY);
//
//        // �ٸ� �÷��̾��� ��ǥ�� ���� ������ ���� ���
//        if (posX >= left && posX <= right &&
//            posY >= top && posY <= bottom)
//        {
//            //=====================================================================================================================================
//            // 3. dfPACKET_SC_DAMAGE �� ��ε�ĳ����
//            //=====================================================================================================================================
//            // 1�� �������� �Ե��� ��
//            pPlayer->Damaged(dfATTACK2_DAMAGE);
//
//            SC_DAMAGE_FOR_All(nullptr, pSession->uid, client->uid, pPlayer->GetHp());
//            break;
//        }
//    }
//
//    return true;
//}
//
//bool CS_ATTACK3(CSession* pSession, UINT8 direction, UINT16 x, UINT16 y)
//{
//    // Ŭ���̾�Ʈ�� ���� ���� �޽����� ����.
//    // g_clientList�� ��ȸ�ϸ� ���� 3�� ������ �����ؼ� �������� �־���.
//    // 1. dfPACKET_SC_ATTACK3 �� ��ε�ĳ����
//    // 2. ���ݹ��� ĳ���͸� �˻�. �˻��� �����ϸ� 3, 4�� ���� ����
//    // 3. dfPACKET_SC_DAMAGE �� ��ε�ĳ����
//    // 4. ���� ü���� 0 ���Ϸ� �������ٸ� dfPACKET_SC_DELETE_CHARACTER �� ��ε�ĳ�����ϰ�, �������� ������ �� �ֵ��� ��
//
//    CPlayer* pPlayer = static_cast<CPlayer*>(pSession->pObj);
//
//    //=====================================================================================================================================
//    // 1. dfPACKET_SC_ATTACK3 �� ��ε�ĳ����
//    //=====================================================================================================================================
//    pPlayer->SetPosition(x, y);
//
//    SC_ATTACK3_FOR_All(pSession, pSession->uid, pPlayer->GetDirection(), x, y);
//
//    //=====================================================================================================================================
//    // 2. ���ݹ��� ĳ���͸� �˻�. �˻��� �����ϸ� 3, 4�� ���� ����
//    //=====================================================================================================================================
//
//    // ���� �ٶ󺸴� ���⿡ ���� ���� ������ �޶���.
//    UINT16 left, right, top, bottom;
//    UINT16 posX, posY;
//    pPlayer->getPosition(posX, posY);
//
//    // ������ �ٶ󺸰� �־��ٸ�
//    if (pPlayer->GetFacingDirection() == dfPACKET_MOVE_DIR_LL)
//    {
//        left = posX - dfATTACK3_RANGE_X;
//        right = posX;
//    }
//    // �������� �ٶ󺸰� �־��ٸ�
//    else
//    {
//        left = posX;
//        right = posX + dfATTACK3_RANGE_X;
//    }
//
//    top = posY - dfATTACK3_RANGE_Y;
//    bottom = posY + dfATTACK3_RANGE_Y;
//
//    for (auto& client : g_clientList)
//    {
//        if (client->uid == pSession->uid)
//            continue;
//
//        pPlayer->getPosition(posX, posY);
//
//        // �ٸ� �÷��̾��� ��ǥ�� ���� ������ ���� ���
//        if (posX >= left && posX <= right &&
//            posY >= top && posY <= bottom)
//        {
//            //=====================================================================================================================================
//            // 3. dfPACKET_SC_DAMAGE �� ��ε�ĳ����
//            //=====================================================================================================================================
//            // 1�� �������� �Ե��� ��
//            pPlayer->Damaged(dfATTACK3_DAMAGE);
//
//            SC_DAMAGE_FOR_All(nullptr, pSession->uid, client->uid, pPlayer->GetHp());
//            break;
//        }
//    }
//
//    return true;
//}

void DisconnectSessionProc(CSession* pSession)
{
    // ������ ������ �ִ� ������Ʈ�� �������� �ʴ´ٸ�, ���ʿ� ������ ���� ��ü�� ���� ���� ���� ���̹Ƿ� �ѱ�
    if (pSession->pObj == nullptr)
        return;

    CPlayer* pPlayer = static_cast<CPlayer*>(pSession->pObj);

    SC_DELETE_CHARACTER_FOR_All(pSession, pPlayer->m_uID);
}
