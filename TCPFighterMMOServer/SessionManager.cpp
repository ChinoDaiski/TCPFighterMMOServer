#include "pch.h"
#include "WinSockManager.h"
#include "SessionManager.h"
#include "Packet.h"

std::list<CSession*> g_clientList;   // ������ ������ ���ǵ鿡 ���� ����

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
    auto it = g_clientList.begin();
    while (it != g_clientList.end())
    {
        // ��Ȱ��ȭ �Ǿ��ٸ�
        if (!(*it)->isAlive)
        {
            // ���������� �־��� �Լ� ȣ��
            m_callbackDisconnect((*it));

            // ����
            closesocket((*it)->sock);
            delete (*it);   // ���� ����

            it = g_clientList.erase(it);
        }
        // Ȱ�� ���̶��
        else
        {
            ++it;
        }
    }
}

void BroadcastData(CSession* excludeCSession, PACKET_HEADER* pPacket, UINT32 dataSize)
{
    for (auto& client : g_clientList)
    {
        // �ش� ������ alive�� �ƴϰų� ������ �����̶�� �Ѿ��
        if (!client->isAlive || excludeCSession == client)
            continue;

        // �޽��� ����, ������ sendQ�� �����͸� ����
        int retVal = client->sendQ.Enqueue((const char*)pPacket, dataSize);

        if (retVal != dataSize)
        {
            // �̷� ���� �־ �ȵ����� Ȥ�� �𸣴� �˻�
            // enqueue���� ������ �� ���� �������� ũ�Ⱑ ����á�ٴ� �ǹ��̹Ƿ�, ���⿡ ���Դٴ� ���� resize�� �ذ� ����.
            // �ٸ�, �������� ũ�Ⱑ ���� �� �� �ִ� ��Ȳ �� tcp ������ ����� 0�� ��쿣 resize�� �ذ��� �ȵ� Ȯ���� ������ ���°� ����. 
            // ���� ������ ���� ����� ���� �׽�Ʈ�ϸ鼭 ����

            NotifyClientDisconnected(client);

            int error = WSAGetLastError();
            std::cout << "Error : BroadcastData(), sendQ�� ����á��" << error << "\n";
            DebugBreak();
        }
    }
}

// Ŭ���̾�Ʈ���� �����͸� ��ε�ĳ��Ʈ�ϴ� �Լ�
void BroadcastData(CSession* excludeCSession, CPacket* pPacket, UINT32 dataSize)
{
    for (auto& client : g_clientList)
    {
        // �ش� ������ alive�� �ƴϰų� ������ �����̶�� �Ѿ��
        if (!client->isAlive || excludeCSession == client)
            continue;

        // �޽��� ����, ������ sendQ�� �����͸� ����
        int retVal = client->sendQ.Enqueue((const char*)pPacket->GetBufferPtr(), dataSize);

        if (retVal != dataSize)
        {
            // �̷� ���� �־ �ȵ����� Ȥ�� �𸣴� �˻�
            // enqueue���� ������ �� ���� �������� ũ�Ⱑ ����á�ٴ� �ǹ��̹Ƿ�, ���⿡ ���Դٴ� ���� resize�� �ذ� ����.
            // �ٸ�, �������� ũ�Ⱑ ���� �� �� �ִ� ��Ȳ �� tcp ������ ����� 0�� ��쿣 resize�� �ذ��� �ȵ� Ȯ���� ������ ���°� ����. 
            // ���� ������ ���� ����� ���� �׽�Ʈ�ϸ鼭 ����

            NotifyClientDisconnected(client);

            int error = WSAGetLastError();
            std::cout << "Error : BroadcastData(), sendQ�� ����á��" << error << "\n";
            DebugBreak();
        }
    }
}

void BroadcastPacket(CSession* excludeCSession, PACKET_HEADER* pHeader, CPacket* pPacket)
{
    BroadcastData(excludeCSession, pHeader, sizeof(PACKET_HEADER));
    BroadcastData(excludeCSession, pPacket, pHeader->bySize);
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

void UnicastData(CSession* includeCSession, PACKET_HEADER* pPacket, UINT32 dataSize)
{
    if (!includeCSession->isAlive)
        return;

    // ������ sendQ�� �����͸� ����
    int retVal = includeCSession->sendQ.Enqueue((const char*)pPacket, dataSize);

    if (retVal != dataSize)
    {
        // �̷� ���� �־ �ȵ����� Ȥ�� �𸣴� �˻�
        // enqueue���� ������ �� ���� �������� ũ�Ⱑ ����á�ٴ� �ǹ��̹Ƿ�, ���⿡ ���Դٴ� ���� resize�� �ذ� ����.
        // �ٸ�, �������� ũ�Ⱑ ���� �� �� �ִ� ��Ȳ �� tcp ������ ����� 0�� ��쿣 resize�� �ذ��� �ȵ� Ȯ���� ������ ���°� ����. 
        // ���� ������ ���� ����� ���� �׽�Ʈ�ϸ鼭 ����

        NotifyClientDisconnected(includeCSession);

        int error = WSAGetLastError();
        std::cout << "Error : UnicastData(), sendQ�� ����á��" << error << "\n";
        DebugBreak();
    }
}

// ���ڷ� ���� ���ǵ鿡�� ������ ������ �õ��ϴ� �Լ�
void UnicastData(CSession* includeCSession, CPacket* pPacket, UINT32 dataSize)
{
    if (!includeCSession->isAlive)
        return;

    // ������ sendQ�� �����͸� ����
    int retVal = includeCSession->sendQ.Enqueue((const char*)pPacket->GetBufferPtr(), dataSize);

    if (retVal != dataSize)
    {
        // �̷� ���� �־ �ȵ����� Ȥ�� �𸣴� �˻�
        // enqueue���� ������ �� ���� �������� ũ�Ⱑ ����á�ٴ� �ǹ��̹Ƿ�, ���⿡ ���Դٴ� ���� resize�� �ذ� ����.
        // �ٸ�, �������� ũ�Ⱑ ���� �� �� �ִ� ��Ȳ �� tcp ������ ����� 0�� ��쿣 resize�� �ذ��� �ȵ� Ȯ���� ������ ���°� ����. 
        // ���� ������ ���� ����� ���� �׽�Ʈ�ϸ鼭 ����

        NotifyClientDisconnected(includeCSession);

        int error = WSAGetLastError();
        std::cout << "Error : UnicastData(), sendQ�� ����á��" << error << "\n";
        DebugBreak();
    }
}

void UnicastPacket(CSession* includeCSession, PACKET_HEADER* pHeader, CPacket* pPacket)
{
    UnicastData(includeCSession, pHeader, sizeof(PACKET_HEADER));
    UnicastData(includeCSession, pPacket, pHeader->bySize);
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