#include "pch.h"
#include "WinSockManager.h"
#include "SessionManager.h"
#include "Packet.h"
#include "SectorManager.h"
#include "ObjectManager.h"

#include "Player.h"

#include "MemoryPoolManager.h"

#include "LogManager.h"

static CSectorManager& sectorManager = CSectorManager::GetInstance();
static CObjectManager& objectManager = CObjectManager::GetInstance();
static LogManager& logManager = LogManager::GetInstance();

std::unordered_map<SOCKET, CSession*> g_SessionHashMap; // 서버에 접속한 세션들에 대한 정보

DisconnectCallback CSessionManager::m_callbackDisconnect;

CSessionManager::CSessionManager() noexcept
{
}

CSessionManager::~CSessionManager() noexcept
{
}


std::string makeDebugLog1(long sectorPosX, long sectorPosY, unsigned char direction, unsigned short posX, unsigned short posY, unsigned char hp)
{
    std::stringstream ss;
    ss << "[ SECTOR : " << sectorPosX << ", " << sectorPosY << " ] [ direction : " << direction << " ] [ curPos : " << posX << ", " << posY << " ] [ hp : " << hp << " ]";

    return ss.str();
}

void CSessionManager::Update(void)
{
    // 비활성화된 클라이언트를 리스트에서 제거
    // 여기서 제거하는 이유는 이전 프레임에 로직 상에서 제거될 세션들의 sendQ가 비워지고 나서 제거되길 원해서 이렇게 작성.
    auto it = g_SessionHashMap.begin();
    while (it != g_SessionHashMap.end())
    {
        // 비활성화 되었다면
        if (!(*it).second->isAlive)
        {
            // 컨텐츠에서 넣어준 함수 호출
            m_callbackDisconnect((*it).second);

            // 제거
            closesocket((*it).second->sock);

            sessionPool.Free((*it).second); // 세션 삭제

            it = g_SessionHashMap.erase(it);
        }
        // 활성 중이라면
        else
        {
            ++it;
        }
    }
}

void BroadcastData(CSession* excludeSession, PACKET_HEADER* pPacketHeader, UINT32 dataSize)
{
    CSession* pSession;
    for (auto& mapIndex : g_SessionHashMap)
    {
        pSession = mapIndex.second;

        // 해당 세션이 alive가 아니거나 제외할 세션이라면 넘어가기
        if (!pSession->isAlive || excludeSession == pSession)
            continue;

        // 메시지 전파, 세션의 sendQ에 데이터를 삽입
        int retVal = pSession->sendQ.Enqueue((const char*)pPacketHeader, dataSize);

        if (retVal != dataSize)
        {
            // 이런 일은 있어선 안되지만 혹시 모르니 검사
            // enqueue에서 문제가 난 것은 링버퍼의 크기가 가득찼다는 의미이므로, 여기에 들어왔다는 것은 resize로 해결 가능.
            // 다만, 링버퍼의 크기가 가득 찰 수 있는 상황 중 tcp 윈도우 사이즈가 0인 경우엔 resize로 해결이 안될 확률이 높으니 끊는게 맞음. 
            // 할지 말지는 오류 생길시 가서 테스트하면서 진행

            int error = WSAGetLastError();
            std::cout << "Error : BroadcastData(), sendQ가 가득찼음" << error << "\n";
            DebugBreak();

            NotifyClientDisconnected(pSession);
        }
    }
}

// 클라이언트에게 데이터를 브로드캐스트하는 함수
void BroadcastData(CSession* excludeSession, CPacket* pPacket, UINT32 dataSize)
{
    CSession* pSession;
    for (auto& mapIndex : g_SessionHashMap)
    {
        pSession = mapIndex.second;

        // 해당 세션이 alive가 아니거나 제외할 세션이라면 넘어가기
        if (!pSession->isAlive || excludeSession == pSession)
            continue;

        // 메시지 전파, 세션의 sendQ에 데이터를 삽입
        int retVal = pSession->sendQ.Enqueue((const char*)pPacket, dataSize);

        if (retVal != dataSize)
        {
            // 이런 일은 있어선 안되지만 혹시 모르니 검사
            // enqueue에서 문제가 난 것은 링버퍼의 크기가 가득찼다는 의미이므로, 여기에 들어왔다는 것은 resize로 해결 가능.
            // 다만, 링버퍼의 크기가 가득 찰 수 있는 상황 중 tcp 윈도우 사이즈가 0인 경우엔 resize로 해결이 안될 확률이 높으니 끊는게 맞음. 
            // 할지 말지는 오류 생길시 가서 테스트하면서 진행

            int error = WSAGetLastError();
            std::cout << "Error : BroadcastData(), sendQ가 가득찼음" << error << "\n";
            DebugBreak();

            NotifyClientDisconnected(pSession);
        }
    }
}

void BroadcastPacket(CSession* excludeSession, PACKET_HEADER* pHeader, CPacket* pPacket)
{
    BroadcastData(excludeSession, pHeader, sizeof(PACKET_HEADER));
    BroadcastData(excludeSession, pPacket, pHeader->bySize);
}

// 클라이언트 연결이 끊어진 경우에 호출되는 함수
void NotifyClientDisconnected(CSession* disconnectedCSession)
{
    //// 만약 이미 죽었다면 NotifyClientDisconnected가 호출되었던 상태이므로 중복인 상태. 체크할 것.
    //if (disconnectedCSession->isAlive == false)
    //{
    //    DebugBreak();
    //}


    // 진짜 잘못된 이유로 인해 문제가 생겼다면 그동안 찍었던 로그를 출력
    // 체력이 0이 됬다거나 timeout으로 인해 먼저 끊어야하는 상황이 아니라면 모두 잘못된 끊어짐

    // 죽지 않았는데 isAlive를 false로 만드는 경우
    if (!disconnectedCSession->pObj->isDead())
    {
        DebugBreak();

        //logManager.LogError(disconnectedCSession->debugLogQueue.rear);

        //// 현재 로그가 어디까지 찍혔는데 출력
        //for (int i = 0; i < 30; ++i)
        //{
        //    logManager.LogError(makeDebugLog1(
        //        std::get<0>(disconnectedCSession->debugLogQueue.queue[i]),
        //        std::get<1>(disconnectedCSession->debugLogQueue.queue[i]),
        //        (unsigned int)std::get<2>(disconnectedCSession->debugLogQueue.queue[i]),
        //        std::get<3>(disconnectedCSession->debugLogQueue.queue[i]),
        //        std::get<4>(disconnectedCSession->debugLogQueue.queue[i]),
        //        (unsigned int)std::get<5>(disconnectedCSession->debugLogQueue.queue[i])
        //    ));
        //}
    }

    disconnectedCSession->isAlive = false;
}

void UnicastData(CSession* includeSession, PACKET_HEADER* pPacket, UINT32 dataSize)
{
    if (!includeSession->isAlive)
        return;

    // 세션의 sendQ에 데이터를 삽입
    int retVal = includeSession->sendQ.Enqueue((const char*)pPacket, dataSize);

    if (retVal != dataSize)
    {
        // 이런 일은 있어선 안되지만 혹시 모르니 검사
        // enqueue에서 문제가 난 것은 링버퍼의 크기가 가득찼다는 의미이므로, 여기에 들어왔다는 것은 resize로 해결 가능.
        // 다만, 링버퍼의 크기가 가득 찰 수 있는 상황 중 tcp 윈도우 사이즈가 0인 경우엔 resize로 해결이 안될 확률이 높으니 끊는게 맞음. 
        // 할지 말지는 오류 생길시 가서 테스트하면서 진행

        int error = WSAGetLastError();
        std::cout << "Error : UnicastData(), sendQ가 가득찼음" << error << "\n";
        DebugBreak();

        NotifyClientDisconnected(includeSession);
    }
}

// 인자로 받은 세션들에게 데이터 전송을 시도하는 함수
void UnicastData(CSession* includeSession, CPacket* pPacket, UINT32 dataSize)
{
    if (!includeSession->isAlive)
        return;

    // 세션의 sendQ에 데이터를 삽입
    int retVal = includeSession->sendQ.Enqueue((const char*)pPacket->GetBufferPtr(), dataSize);

    if (retVal != dataSize)
    {
        // 이런 일은 있어선 안되지만 혹시 모르니 검사
        // enqueue에서 문제가 난 것은 링버퍼의 크기가 가득찼다는 의미이므로, 여기에 들어왔다는 것은 resize로 해결 가능.
        // 다만, 링버퍼의 크기가 가득 찰 수 있는 상황 중 tcp 윈도우 사이즈가 0인 경우엔 resize로 해결이 안될 확률이 높으니 끊는게 맞음. 
        // 할지 말지는 오류 생길시 가서 테스트하면서 진행

        int error = WSAGetLastError();
        std::cout << "Error : UnicastData(), sendQ가 가득찼음" << error << "\n";
        DebugBreak();

        NotifyClientDisconnected(includeSession);
    }
}

void UnicastPacket(CSession* includeSession, PACKET_HEADER* pHeader, CPacket* pPacket)
{
    UnicastData(includeSession, pHeader, sizeof(PACKET_HEADER));
    UnicastData(includeSession, pPacket, pHeader->bySize);
}

CSession* createSession(SOCKET ClientSocket, SOCKADDR_IN ClientAddr)
{
    CWinSockManager& winSockManager = CWinSockManager::GetInstance();

    // accept가 완료되었다면 세션에 등록 후, 해당 세션에 패킷 전송
    CSession* Session = sessionPool.Alloc();
    Session->recvQ.ClearBuffer();
    Session->sendQ.ClearBuffer();

    Session->isAlive = true;

    // 소켓 정보 추가
    Session->sock = ClientSocket;

    // IP / PORT 정보 추가
    memcpy(Session->IP, winSockManager.GetIP(ClientAddr).c_str(), sizeof(Session->IP));
    Session->port = winSockManager.GetPort(ClientAddr);

    // 세션이 가지고 있을 객체 포인터 정보 초기화
    Session->pObj = nullptr;

    return Session;
}