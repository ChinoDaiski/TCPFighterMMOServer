#include "pch.h"
#include "WinSockManager.h"
#include "SessionManager.h"
#include "Packet.h"

std::unordered_map<SOCKET, CSession*> g_SessionHashMap; // 서버에 접속한 세션들에 대한 정보

DisconnectCallback CSessionManager::m_callbackDisconnect;

CSessionManager::CSessionManager() noexcept
{
}

CSessionManager::~CSessionManager() noexcept
{
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
            delete (*it).second;   // 세션 삭제

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

            NotifyClientDisconnected(pSession);

            int error = WSAGetLastError();
            std::cout << "Error : BroadcastData(), sendQ가 가득찼음" << error << "\n";
            DebugBreak();
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

            NotifyClientDisconnected(pSession);

            int error = WSAGetLastError();
            std::cout << "Error : BroadcastData(), sendQ가 가득찼음" << error << "\n";
            DebugBreak();
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
    // 만약 이미 죽었다면 NotifyClientDisconnected가 호출되었던 상태이므로 중복인 상태. 체크할 것.
    if (disconnectedCSession->isAlive == false)
    {
        DebugBreak();
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

        NotifyClientDisconnected(includeSession);

        int error = WSAGetLastError();
        std::cout << "Error : UnicastData(), sendQ가 가득찼음" << error << "\n";
        DebugBreak();
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

        NotifyClientDisconnected(includeSession);

        int error = WSAGetLastError();
        std::cout << "Error : UnicastData(), sendQ가 가득찼음" << error << "\n";
        DebugBreak();
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
    CSession* Session = new CSession;
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