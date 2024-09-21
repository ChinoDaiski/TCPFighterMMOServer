
#include "pch.h"
#include "WinSockManager.h"

CWinSockManager::CWinSockManager() noexcept
{
    m_listenSocket = INVALID_SOCKET;
}

CWinSockManager::~CWinSockManager() noexcept
{
    Cleanup();
}

void CWinSockManager::InitWinSock() noexcept
{
    // ������ ���� �ʱ�ȭ
    WSADATA wsaData;

    int retVal = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (retVal != 0) {
        std::cout << "Error : WSAStartup failed " << retVal << ", " << WSAGetLastError() << "\n";
        DebugBreak();
    }
}

void CWinSockManager::CreateListenSocket(PROTOCOL_TYPE type)
{
    switch (type)
    {
    case PROTOCOL_TYPE::TCP_IP:
        // �̹� 2���ڿ��� TCP�� UDP�� ������ ���� ������ 3���ڿ� ���� �ʾƵ� �����ϴ�.
        m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_listenSocket == INVALID_SOCKET)
        {
            std::cout << "Error : TCP CreateListenSocket()" << WSAGetLastError() << "\n";
            DebugBreak();
        }
        break;
    case PROTOCOL_TYPE::UDP:
        m_listenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_listenSocket == INVALID_SOCKET)
        {
            std::cout << "Error : UDP CreateListenSocket()" << WSAGetLastError() << "\n";
            DebugBreak();
        }
        break;
    default:
        std::cout << "Error : default socket()" << WSAGetLastError() << "\n";
        DebugBreak();
        break;
    }
}

void CWinSockManager::Bind(UINT16 serverPort, UINT32 serverIP)
{
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(serverIP);
    serveraddr.sin_port = htons(serverPort);
    int retVal = bind(m_listenSocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

    if (retVal == SOCKET_ERROR)
    {
        std::cout << "Error : Bind(), " << WSAGetLastError() << "\n";
        DebugBreak();
    }
}

void CWinSockManager::Listen(UINT32 somaxconn)
{
    int retVal = listen(m_listenSocket, somaxconn);
    if (retVal == SOCKET_ERROR)
    {
        std::cout << "Error : Listen(), " << WSAGetLastError() << "\n";
        DebugBreak();
    }
}

inline void CWinSockManager::SetOptions(UINT8 options)
{
    if (options == 0)
        return;

    if (options | OPTION_NONBLOCKING)
    {
        ULONG on = 1;   // 0�� �ƴ� ��� non-blocking �𵨷� ��ȯ
        int NonBlockingRetVal = ioctlsocket(m_listenSocket, FIONBIO, &on);
        if (NonBlockingRetVal == SOCKET_ERROR)
        {
            std::cout << "Error : SetOptions(), OPTION_NONBLOCKING. �� ���ŷ ��ȯ ����. " << WSAGetLastError() << "\n";
        }
    }
}

void CWinSockManager::StartServer(PROTOCOL_TYPE type, UINT16 serverPort, UINT8 options, UINT32 serverIP, UINT32 somaxconn) noexcept {

    // ������ �ʱ�ȭ
    InitWinSock();

    // listen ���� ����
    CreateListenSocket(type);

    // bind
    Bind(serverPort, serverIP);

    // option ����
    SetOptions(options);

    // listen
    Listen(somaxconn);
}

SOCKET CWinSockManager::Accept(SOCKADDR_IN& ClientAddr) noexcept
{
    // accept ����
    int addrlen = sizeof(ClientAddr);
    SOCKET ClientSocket = accept(m_listenSocket, (SOCKADDR*)&ClientAddr, &addrlen);

    if (ClientSocket == INVALID_SOCKET)
    {
        std::cout << "Error : Accept(), " << WSAGetLastError() << "\n";
    }
    // accept�� ������ �Ҳ��� WOULDBLOCK�� ���� �� ���� �ݺ��ؼ� accept�� �õ��ؾ��Ѵ�. ������ ���⼱ 1ȸ�� �� ���̹Ƿ� INVALID ó���� �ϰ� �Ѿ.

    // ������ Ŭ���̾�Ʈ ������ ���� �ʹٸ�
    // std::cout<<"[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = % s, ��Ʈ ��ȣ = % d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

    return ClientSocket;
}

std::string CWinSockManager::GetIP(const SOCKADDR_IN& ClientAddr) noexcept
{
    return GetIP(ClientAddr.sin_addr);
}

std::string CWinSockManager::GetIP(const IN_ADDR& ClientInAddr) noexcept
{
    char pAddrBuf[INET_ADDRSTRLEN];

    // ���̳ʸ� �ּҸ� ����� ���� �� �ִ� ���·� ��ȯ
    if (inet_ntop(AF_INET, &ClientInAddr, pAddrBuf, sizeof(pAddrBuf)) == NULL) {
        std::cout << "Error : inet_ntop()\n";
        DebugBreak();
    }

    char strIP[16];
    memcpy(strIP, pAddrBuf, sizeof(char) * 16);

    return std::string{ strIP };
}

UINT16 CWinSockManager::GetPort(const SOCKADDR_IN& ClientAddr) noexcept
{
    return GetPort(ClientAddr.sin_port);
}

UINT16 CWinSockManager::GetPort(const UINT16& port) noexcept
{
    return ntohs(port);
}

void CWinSockManager::Cleanup() noexcept
{
    if (m_listenSocket != INVALID_SOCKET) {
        closesocket(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
    }

    WSACleanup();
}
