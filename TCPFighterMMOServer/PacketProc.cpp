#include "pch.h"
#include "PacketProc.h"
#include "Packet.h"
#include "MakePacket.h"

#include "SessionManager.h"

#include "ObjectManager.h"

#include "SectorManager.h"
#include "Sector.h"

#include "Player.h"

#include "MemoryPoolManager.h"
#include "TimerManager.h"
#include "LogManager.h"


static CSectorManager& sectorManager = CSectorManager::GetInstance();
static CObjectManager& objectManager = CObjectManager::GetInstance();
static CTimerManager& timerManager = CTimerManager::GetInstance();
static LogManager& logManager = LogManager::GetInstance();

int g_iSyncCount = 0;


std::string makeDebugLog(long sectorPosX, long sectorPosY, unsigned char direction, unsigned short posX, unsigned short posY, unsigned int hp)
{
    std::stringstream ss;
    ss << "[ SECTOR : " << sectorPosX << ", " << sectorPosY << " ] [ direction : " << direction << " ] [ curPos : " << posX << ", " << posY << " ] [ hp : " << hp << " ]";

    return ss.str();
}

bool PacketProc(CSession* pSession, PACKET_TYPE packetType, CPacket* pPacket)
{
    //pSession->pObj->m_packetQueue.enqueue(std::make_tuple(packetType, timerManager.GetCurrServerTime(), pSession->SessionID));

    switch (packetType)
    {
    case PACKET_TYPE::CS_MOVE_START:
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
    break;
    case PACKET_TYPE::CS_ECHO:
    {
        UINT32		Time;

        *pPacket >> Time;

        return CS_ECHO(pSession, Time);
    }
    break;
    case PACKET_TYPE::CS_SYNC:
    {
        UINT16 X;
        UINT16 Y;

        *pPacket >> X;
        *pPacket >> Y;

        return CS_SYNC(pSession, X, Y);
    }
    break;
    default:
        return false;
        break;
    }
    return false;
}

bool CS_MOVE_START(CSession* pSession, UINT8 direction, UINT16 x, UINT16 y)
{
    // 메시지 수신 로그 확인
    // ==========================================================================================================
    // 서버의 위치와 받은 패킷의 위치값이 너무 큰 차이가 난다면 끊어버림                           .
    // 본 게임의 좌표 동기화 구조는 단순한 키보드 조작 (클라어안트의 션처리, 서버의 후 반영) 방식으로
    // 클라이언트의 좌표를 그대로 믿는 방식을 택하고 있음.
    // 실제 온라인 게임이라면 클라이언트에서 목적지를 공유하는 방식을 택해야함.
    // 지금은 간단한 구현을 목적으로 하고 있으므로 오차범위 내에서 클라이언트 좌표를 믿도록 한다.
    // ==========================================================================================================

    CPlayer* pPlayer = static_cast<CPlayer*>(pSession->pObj);

    UINT16 posX, posY;
    pPlayer->getPosition(posX, posY);


    
    pSession->debugLogQueue.enqueue(std::make_tuple(
        pSession->pObj->m_curSectorPos.x,
        pSession->pObj->m_curSectorPos.y,
        direction,
        x,
        y,
        static_cast<CPlayer*>(pSession->pObj)->GetHp()
    ));




    // 클라이언트의 좌표가 서버와 잘못되었지만 서버의 좌표가 맞다고 봐야하기 때문에 싱크 메시지에 서버가 관리하는 좌표를 넣어 클라가 맞추게 한다.
    if (
        std::abs(posX - x) > dfERROR_RANGE ||
        std::abs(posY - y) > dfERROR_RANGE
        )
    {
        // 클라에게 서버에서 플레이어의 위치를 보낸다.
        SC_SYNC_FOR_SINGLE(pSession, pPlayer->m_ID, posX, posY); 
        g_iSyncCount++;

        // 싱크가 난 이유를 찾기 위해 지터를 측정
        timerManager.PrintJitterStats();

        // 서버와 클라에서 플레이어의 위치와 방향을 찍어서 어떻게 싱크가 났는지 확인.
        logManager.LogDebug(
            "Player\n"
            "Server\nPos : ", posX, ", ", posY, "  Direction : ", static_cast<int>(pPlayer->GetDirection()), "\n"
            "Client\nPos : ", x, ", ", y, "  Direction : ", static_cast<int>(direction), "\n"
            "Pre Sector Index : ", pPlayer->m_preSectorPos.x, ", ", pPlayer->m_preSectorPos.x, "\n"
            "Cur Sector Index : ", pPlayer->m_curSectorPos.x, ", ", pPlayer->m_curSectorPos.x, "\n"
            "Cur Sector PlayerCnt : ", sectorManager.GetSectorInfo(pPlayer->m_curSectorPos.x, pPlayer->m_curSectorPos.y)->GetSectorObjectMap().size(), "\n\n"
        );
    }
    // 그다지 오차가 크기 않을 경우 클라의 위치를 서버가 믿어준다.
    else
    {
        // ==========================================================================================================
        // 동작을 변경. 지금 구현에선 동작번호가 방향값. 내부에서 바라보는 방향도 변경
        // ==========================================================================================================
        pPlayer->SetPosition(x, y);
        pPlayer->SetDirection(direction);
    }


    // ==========================================================================================================
    // 당사자를 제외한, 현재 접속중인 모든 사용자에게 패킷을 뿌림.
    // ==========================================================================================================

    // 현재 오브젝트가 위치한 섹터정보 추출
    CSector* pCurSector = sectorManager.GetSectorInfo(pPlayer->m_curSectorPos.x, pPlayer->m_curSectorPos.y);
    SC_MOVE_START_FOR_AROUND(pSession, pCurSector, pPlayer->m_ID, pPlayer->GetDirection(), x, y);

    //=====================================================================================================================================
    // 이동 연산 시작을 알림
    //=====================================================================================================================================
    pPlayer->SetFlag(FLAG_MOVING, true);

    return true;
}

bool CS_MOVE_STOP(CSession* pSession, UINT8 direction, UINT16 x, UINT16 y)
{
    // 현재 선택된 클라이언트가 서버에게 움직임을 멈출 것이라 요청
    // 1. 받은 데이터 처리
    // 2. PACKET_SC_MOVE_STOP 을 브로드캐스팅
    // 3. 서버 내에서 이동 연산 멈춤을 알림

    CPlayer* pPlayer = static_cast<CPlayer*>(pSession->pObj);

    UINT16 posX, posY;
    pPlayer->getPosition(posX, posY);


    pSession->debugLogQueue.enqueue(std::make_tuple(
        pSession->pObj->m_curSectorPos.x,
        pSession->pObj->m_curSectorPos.y,
        direction,
        x,
        y,
        static_cast<CPlayer*>(pSession->pObj)->GetHp()
    ));




    // 클라이언트의 좌표가 서버와 잘못되었지만 서버의 좌표가 맞다고 봐야하기 때문에 싱크 메시지에 서버가 관리하는 좌표를 넣어 클라가 맞추게 한다.
    if (
        std::abs(posX - x) > dfERROR_RANGE ||
        std::abs(posY - y) > dfERROR_RANGE
        )
    {
        // 클라에게 서버에서 플레이어의 위치를 보낸다.
        SC_SYNC_FOR_SINGLE(pSession, pPlayer->m_ID, posX, posY);
        g_iSyncCount++;

        // 싱크가 난 이유를 찾기 위해 지터를 측정
        timerManager.PrintJitterStats();

        // 서버와 클라에서 플레이어의 위치와 방향을 찍어서 어떻게 싱크가 났는지 확인.
        logManager.LogDebug(
            "Player\n"
            "Server\nPos : ", posX, ", ", posY, "  Direction : ", static_cast<int>(pPlayer->GetDirection()), "\n"
            "Client\nPos : ", x, ", ", y, "  Direction : ", static_cast<int>(direction), "\n"
            "Pre Sector Index : ", pPlayer->m_preSectorPos.x, ", ", pPlayer->m_preSectorPos.x, "\n"
            "Cur Sector Index : ", pPlayer->m_curSectorPos.x, ", ", pPlayer->m_curSectorPos.x, "\n"
            "Cur Sector PlayerCnt : ", sectorManager.GetSectorInfo(pPlayer->m_curSectorPos.x, pPlayer->m_curSectorPos.y)->GetSectorObjectMap().size(), "\n\n"
        );
    }
    // 그다지 오차가 크기 않을 경우 클라의 위치를 서버가 믿어준다.
    else
    {
        //=====================================================================================================================================
        // 1. 받은 데이터 처리
        //=====================================================================================================================================
        // 클라의 위치를 서버에서 맞추기 위함 
        pPlayer->SetDirection(direction);
        pPlayer->SetPosition(x, y);
    }

    //=====================================================================================================================================
    // 2. PACKET_SC_MOVE_STOP 를 브로드캐스팅
    //=====================================================================================================================================

    // 현재 오브젝트가 위치한 섹터정보 추출
    CSector* pCurSector = sectorManager.GetSectorInfo(pPlayer->m_curSectorPos.x, pPlayer->m_curSectorPos.y);
    SC_MOVE_STOP_FOR_AROUND(pSession, pCurSector, pPlayer->m_ID, pPlayer->GetDirection(), x, y);

    //=====================================================================================================================================
    // 3. 서버 내에서 이동 연산 멈춤을 알림
    //=====================================================================================================================================
    pPlayer->SetFlag(FLAG_MOVING, false);

    return true;
}

bool CS_ATTACK1(CSession* pSession, UINT8 direction, UINT16 x, UINT16 y)
{
    // 클라이언트로 부터 공격 메시지가 들어옴.
    // g_clientList를 순회하며 공격 1의 범위를 연산해서 데미지를 넣어줌.
    // 1. dfPACKET_SC_ATTACK1 을 브로드캐스팅
    // 2. 공격받을 캐릭터를 검색. 검색에 성공하면 3, 4번 절차 진행
    // 3. dfPACKET_SC_DAMAGE 를 브로드캐스팅
    // 4. 만약 체력이 0 이하로 떨어졌다면 dfPACKET_SC_DELETE_CHARACTER 를 브로드캐스팅하고, 서버에서 삭제할 수 있도록 함 -> 이 부분은 로직에서 처리하도록 바꿈.

    CPlayer* pPlayer = static_cast<CPlayer*>(pSession->pObj);

    //=====================================================================================================================================
    // 1. dfPACKET_SC_ATTACK1 을 브로드캐스팅
    //=====================================================================================================================================
    //pPlayer->SetPosition(x, y);

    CSector* pCurSector = sectorManager.GetSectorInfo(pPlayer->m_curSectorPos.x, pPlayer->m_curSectorPos.y);
    SC_ATTACK1_FOR_AROUND(pSession, pCurSector, pPlayer->m_ID, pPlayer->GetDirection(), x, y);

    //=====================================================================================================================================
    // 2. 공격받을 캐릭터를 검색. 검색에 성공하면 3, 4번 절차 진행
    //=====================================================================================================================================

    // 내가 바라보는 방향에 따라 공격 범위가 달라짐.
    UINT16 left, right, top, bottom;
    UINT16 posX, posY;
    pPlayer->getPosition(posX, posY);

    // 왼쪽을 바라보고 있었다면
    if (pPlayer->GetFacingDirection() == dfPACKET_MOVE_DIR_LL)
    {
        left = posX - dfATTACK1_RANGE_X;
        right = posX;
    }
    // 오른쪽을 바라보고 있었다면
    else
    {
        left = posX;
        right = posX + dfATTACK1_RANGE_X;
    }

    top = posY - dfATTACK1_RANGE_Y;
    bottom = posY + dfATTACK1_RANGE_Y;

    CPlayer* pOtherPlayer;
   
    for (auto& Sector : pCurSector->GetAroundSectorList())
    {
        bool bBreak = false;
        for (auto& Object : Sector->GetSectorObjectMap())
        {
            pOtherPlayer = static_cast<CPlayer*>(Object.second);

            if (pPlayer == pOtherPlayer)
                continue;

            pOtherPlayer->getPosition(posX, posY);

            // 다른 플레이어의 좌표가 공격 범위에 있을 경우
            if (posX >= left && posX <= right &&
                posY >= top && posY <= bottom)
            {
                //=====================================================================================================================================
                // 3. dfPACKET_SC_DAMAGE 를 브로드캐스팅
                //=====================================================================================================================================
                // 1명만 데미지를 입도록 함
                pOtherPlayer->Damaged(dfATTACK1_DAMAGE);

                SC_DAMAGE_FOR_AROUND(nullptr, pCurSector, pSession->pObj->m_ID, pOtherPlayer->m_ID, pOtherPlayer->GetHp());
                bBreak = true;
                break;
            }
        }

        if (bBreak)
            break;
    }

    return true;
}

bool CS_ATTACK2(CSession* pSession, UINT8 direction, UINT16 x, UINT16 y)
{
    // 클라이언트로 부터 공격 메시지가 들어옴.
    // g_clientList를 순회하며 공격 2의 범위를 연산해서 데미지를 넣어줌.
    // 1. dfPACKET_SC_ATTACK2 을 브로드캐스팅
    // 2. 공격받을 캐릭터를 검색. 검색에 성공하면 3, 4번 절차 진행
    // 3. dfPACKET_SC_DAMAGE 를 브로드캐스팅
    // 4. 만약 체력이 0 이하로 떨어졌다면 dfPACKET_SC_DELETE_CHARACTER 를 브로드캐스팅하고, 서버에서 삭제할 수 있도록 함

    CPlayer* pPlayer = static_cast<CPlayer*>(pSession->pObj);

    //=====================================================================================================================================
    // 1. dfPACKET_SC_ATTACK2 을 브로드캐스팅
    //=====================================================================================================================================
    //pPlayer->SetPosition(x, y);

    CSector* pCurSector = sectorManager.GetSectorInfo(pPlayer->m_curSectorPos.x, pPlayer->m_curSectorPos.y);
    SC_ATTACK2_FOR_AROUND(pSession, pCurSector, pPlayer->m_ID, pPlayer->GetDirection(), x, y);

    //=====================================================================================================================================
    // 2. 공격받을 캐릭터를 검색. 검색에 성공하면 3, 4번 절차 진행
    //=====================================================================================================================================

    // 내가 바라보는 방향에 따라 공격 범위가 달라짐.
    UINT16 left, right, top, bottom;
    UINT16 posX, posY;
    pPlayer->getPosition(posX, posY);

    // 왼쪽을 바라보고 있었다면
    if (pPlayer->GetFacingDirection() == dfPACKET_MOVE_DIR_LL)
    {
        left = posX - dfATTACK2_RANGE_X;
        right = posX;
    }
    // 오른쪽을 바라보고 있었다면
    else
    {
        left = posX;
        right = posX + dfATTACK2_RANGE_X;
    }

    top = posY - dfATTACK2_RANGE_Y;
    bottom = posY + dfATTACK2_RANGE_Y;

    CPlayer* pOtherPlayer;
    for (auto& Sector : pCurSector->GetAroundSectorList())
    {
        bool bBreak = false;
        for (auto& Object : Sector->GetSectorObjectMap())
        {
            pOtherPlayer = static_cast<CPlayer*>(Object.second);

            if (pPlayer == pOtherPlayer)
                continue;

            pOtherPlayer->getPosition(posX, posY);

            // 다른 플레이어의 좌표가 공격 범위에 있을 경우
            if (posX >= left && posX <= right &&
                posY >= top && posY <= bottom)
            {
                //=====================================================================================================================================
                // 3. dfPACKET_SC_DAMAGE 를 브로드캐스팅
                //=====================================================================================================================================
                // 1명만 데미지를 입도록 함
                pOtherPlayer->Damaged(dfATTACK2_DAMAGE);

                SC_DAMAGE_FOR_AROUND(nullptr, pCurSector, pSession->pObj->m_ID, pOtherPlayer->m_ID, pOtherPlayer->GetHp());
                bBreak = true;
                break;
            }
        }

        if (bBreak)
            break;
    }

    return true;
}

bool CS_ATTACK3(CSession* pSession, UINT8 direction, UINT16 x, UINT16 y)
{
    // 클라이언트로 부터 공격 메시지가 들어옴.
    // g_clientList를 순회하며 공격 3의 범위를 연산해서 데미지를 넣어줌.
    // 1. dfPACKET_SC_ATTACK3 을 브로드캐스팅
    // 2. 공격받을 캐릭터를 검색. 검색에 성공하면 3, 4번 절차 진행
    // 3. dfPACKET_SC_DAMAGE 를 브로드캐스팅
    // 4. 만약 체력이 0 이하로 떨어졌다면 dfPACKET_SC_DELETE_CHARACTER 를 브로드캐스팅하고, 서버에서 삭제할 수 있도록 함

    CPlayer* pPlayer = static_cast<CPlayer*>(pSession->pObj);

    //=====================================================================================================================================
    // 1. dfPACKET_SC_ATTACK3 을 브로드캐스팅
    //=====================================================================================================================================
    //pPlayer->SetPosition(x, y);

    CSector* pCurSector = sectorManager.GetSectorInfo(pPlayer->m_curSectorPos.x, pPlayer->m_curSectorPos.y);
    SC_ATTACK3_FOR_AROUND(pSession, pCurSector, pPlayer->m_ID, pPlayer->GetDirection(), x, y);

    //=====================================================================================================================================
    // 2. 공격받을 캐릭터를 검색. 검색에 성공하면 3, 4번 절차 진행
    //=====================================================================================================================================

    // 내가 바라보는 방향에 따라 공격 범위가 달라짐.
    UINT16 left, right, top, bottom;
    UINT16 posX, posY;
    pPlayer->getPosition(posX, posY);

    // 왼쪽을 바라보고 있었다면
    if (pPlayer->GetFacingDirection() == dfPACKET_MOVE_DIR_LL)
    {
        left = posX - dfATTACK3_RANGE_X;
        right = posX;
    }
    // 오른쪽을 바라보고 있었다면
    else
    {
        left = posX;
        right = posX + dfATTACK3_RANGE_X;
    }

    top = posY - dfATTACK3_RANGE_Y;
    bottom = posY + dfATTACK3_RANGE_Y;

    CPlayer* pOtherPlayer;
    for (auto& Sector : pCurSector->GetAroundSectorList())
    {
        bool bBreak = false;
        for (auto& Object : Sector->GetSectorObjectMap())
        {
            pOtherPlayer = static_cast<CPlayer*>(Object.second);

            if (pPlayer == pOtherPlayer)
                continue;

            pOtherPlayer->getPosition(posX, posY);

            // 다른 플레이어의 좌표가 공격 범위에 있을 경우
            if (posX >= left && posX <= right &&
                posY >= top && posY <= bottom)
            {
                //=====================================================================================================================================
                // 3. dfPACKET_SC_DAMAGE 를 브로드캐스팅
                //=====================================================================================================================================
                // 1명만 데미지를 입도록 함
                pOtherPlayer->Damaged(dfATTACK3_DAMAGE);

                SC_DAMAGE_FOR_AROUND(nullptr, pCurSector, pSession->pObj->m_ID, pOtherPlayer->m_ID, pOtherPlayer->GetHp());
                bBreak = true;
                break;
            }
        }

        if (bBreak)
            break;
    }

    return true;
}

bool CS_ECHO(CSession* pSession, UINT32 time)
{
    SC_ECHO_FOR_SINGLE(pSession, time);

    return true;
}

bool CS_SYNC(CSession* pSession, UINT16 x, UINT16 y)
{
    // 아무것도 없음

    return true;
}

void DisconnectSessionProc(CSession* pSession)
{
    // 세션이 가지고 있는 오브젝트가 존재하지 않는다면, 애초에 연결을 위한 객체가 생성 되지 않은 것이므로 넘김
    if (pSession->pObj == nullptr)
        return;

    CPlayer* pPlayer = static_cast<CPlayer*>(pSession->pObj);

    CSector* pCurSector = sectorManager.GetSectorInfo(pPlayer->m_curSectorPos.x, pPlayer->m_curSectorPos.y);
    SC_DELETE_CHARACTER_FOR_AROUND(nullptr, pCurSector, pPlayer->m_ID);

    sectorManager.DeleteObjectFromSector(pSession->pObj); // 섹터 매니저에서 삭제
    objectManager.DeleteObject(pSession->pObj); // 오브젝트 매니저에서 삭제

    playerPool.Free(static_cast<CPlayer*>(pSession->pObj)); // 플레이어 삭제
}
