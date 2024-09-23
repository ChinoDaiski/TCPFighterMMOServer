#include "pch.h"
#include "WinSockManager.h"
#include "SessionManager.h"
#include "Packet.h"

std::unordered_map<SOCKET, CSession*> g_SessionHashMap; // ������ ������ ���ǵ鿡 ���� ����

DisconnectCallback CSessionManager::m_callbackDisconnect;

CSessionManager::CSessionManager() noexcept
{
}

CSessionManager::~CSessionManager() noexcept
{
}

void CSessionManager::Update(void)
{
    // ��Ȱ��ȭ�� Ŭ���̾�Ʈ�� ����Ʈ���� ����
    // ���⼭ �����ϴ� ������ ���� �����ӿ� ���� �󿡼� ���ŵ� ���ǵ��� sendQ�� ������� ���� ���ŵǱ� ���ؼ� �̷��� �ۼ�.
    auto it = g_SessionHashMap.begin();
    while (it != g_SessionHashMap.end())
    {
        // ��Ȱ��ȭ �Ǿ��ٸ�
        if (!(*it).second->isAlive)
        {
            // ���������� �־��� �Լ� ȣ��
            m_callbackDisconnect((*it).second);

            // ����
            closesocket((*it).second->sock);
            delete (*it).second;   // ���� ����

            it = g_SessionHashMap.erase(it);
        }
        // Ȱ�� ���̶��
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

        // �ش� ������ alive�� �ƴϰų� ������ �����̶�� �Ѿ��
        if (!pSession->isAlive || excludeSession == pSession)
            continue;

        // �޽��� ����, ������ sendQ�� �����͸� ����
        int retVal = pSession->sendQ.Enqueue((const char*)pPacketHeader, dataSize);

        if (retVal != dataSize)
        {
            // �̷� ���� �־ �ȵ����� Ȥ�� �𸣴� �˻�
            // enqueue���� ������ �� ���� �������� ũ�Ⱑ ����á�ٴ� �ǹ��̹Ƿ�, ���⿡ ���Դٴ� ���� resize�� �ذ� ����.
            // �ٸ�, �������� ũ�Ⱑ ���� �� �� �ִ� ��Ȳ �� tcp ������ ����� 0�� ��쿣 resize�� �ذ��� �ȵ� Ȯ���� ������ ���°� ����. 
            // ���� ������ ���� ����� ���� �׽�Ʈ�ϸ鼭 ����

            NotifyClientDisconnected(pSession);

            int error = WSAGetLastError();
            std::cout << "Error : BroadcastData(), sendQ�� ����á��" << error << "\n";
            DebugBreak();
        }
    }
}

// Ŭ���̾�Ʈ���� �����͸� ��ε�ĳ��Ʈ�ϴ� �Լ�
void BroadcastData(CSession* excludeSession, CPacket* pPacket, UINT32 dataSize)
{
    CSession* pSession;
    for (auto& mapIndex : g_SessionHashMap)
    {
        pSession = mapIndex.second;

        // �ش� ������ alive�� �ƴϰų� ������ �����̶�� �Ѿ��
        if (!pSession->isAlive || excludeSession == pSession)
            continue;

        // �޽��� ����, ������ sendQ�� �����͸� ����
        int retVal = pSession->sendQ.Enqueue((const char*)pPacket, dataSize);

        if (retVal != dataSize)
        {
            // �̷� ���� �־ �ȵ����� Ȥ�� �𸣴� �˻�
            // enqueue���� ������ �� ���� �������� ũ�Ⱑ ����á�ٴ� �ǹ��̹Ƿ�, ���⿡ ���Դٴ� ���� resize�� �ذ� ����.
            // �ٸ�, �������� ũ�Ⱑ ���� �� �� �ִ� ��Ȳ �� tcp ������ ����� 0�� ��쿣 resize�� �ذ��� �ȵ� Ȯ���� ������ ���°� ����. 
            // ���� ������ ���� ����� ���� �׽�Ʈ�ϸ鼭 ����

            NotifyClientDisconnected(pSession);

            int error = WSAGetLastError();
            std::cout << "Error : BroadcastData(), sendQ�� ����á��" << error << "\n";
            DebugBreak();
        }
    }
}

void BroadcastPacket(CSession* excludeSession, PACKET_HEADER* pHeader, CPacket* pPacket)
{
    BroadcastData(excludeSession, pHeader, sizeof(PACKET_HEADER));
    BroadcastData(excludeSession, pPacket, pHeader->bySize);
}

// Ŭ���̾�Ʈ ������ ������ ��쿡 ȣ��Ǵ� �Լ�
void NotifyClientDisconnected(CSession* disconnectedCSession)
{
    // ���� �̹� �׾��ٸ� NotifyClientDisconnected�� ȣ��Ǿ��� �����̹Ƿ� �ߺ��� ����. üũ�� ��.
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

    // ������ sendQ�� �����͸� ����
    int retVal = includeSession->sendQ.Enqueue((const char*)pPacket, dataSize);

    if (retVal != dataSize)
    {
        // �̷� ���� �־ �ȵ����� Ȥ�� �𸣴� �˻�
        // enqueue���� ������ �� ���� �������� ũ�Ⱑ ����á�ٴ� �ǹ��̹Ƿ�, ���⿡ ���Դٴ� ���� resize�� �ذ� ����.
        // �ٸ�, �������� ũ�Ⱑ ���� �� �� �ִ� ��Ȳ �� tcp ������ ����� 0�� ��쿣 resize�� �ذ��� �ȵ� Ȯ���� ������ ���°� ����. 
        // ���� ������ ���� ����� ���� �׽�Ʈ�ϸ鼭 ����

        NotifyClientDisconnected(includeSession);

        int error = WSAGetLastError();
        std::cout << "Error : UnicastData(), sendQ�� ����á��" << error << "\n";
        DebugBreak();
    }
}

// ���ڷ� ���� ���ǵ鿡�� ������ ������ �õ��ϴ� �Լ�
void UnicastData(CSession* includeSession, CPacket* pPacket, UINT32 dataSize)
{
    if (!includeSession->isAlive)
        return;

    // ������ sendQ�� �����͸� ����
    int retVal = includeSession->sendQ.Enqueue((const char*)pPacket->GetBufferPtr(), dataSize);

    if (retVal != dataSize)
    {
        // �̷� ���� �־ �ȵ����� Ȥ�� �𸣴� �˻�
        // enqueue���� ������ �� ���� �������� ũ�Ⱑ ����á�ٴ� �ǹ��̹Ƿ�, ���⿡ ���Դٴ� ���� resize�� �ذ� ����.
        // �ٸ�, �������� ũ�Ⱑ ���� �� �� �ִ� ��Ȳ �� tcp ������ ����� 0�� ��쿣 resize�� �ذ��� �ȵ� Ȯ���� ������ ���°� ����. 
        // ���� ������ ���� ����� ���� �׽�Ʈ�ϸ鼭 ����

        NotifyClientDisconnected(includeSession);

        int error = WSAGetLastError();
        std::cout << "Error : UnicastData(), sendQ�� ����á��" << error << "\n";
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

    // accept�� �Ϸ�Ǿ��ٸ� ���ǿ� ��� ��, �ش� ���ǿ� ��Ŷ ����
    CSession* Session = new CSession;
    Session->isAlive = true;

    // ���� ���� �߰�
    Session->sock = ClientSocket;

    // IP / PORT ���� �߰�
    memcpy(Session->IP, winSockManager.GetIP(ClientAddr).c_str(), sizeof(Session->IP));
    Session->port = winSockManager.GetPort(ClientAddr);

    // ������ ������ ���� ��ü ������ ���� �ʱ�ȭ
    Session->pObj = nullptr;

    return Session;
}