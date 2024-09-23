#include "pch.h"
#include "NetIOManager.h"

#include "WinSockManager.h"

#include "Session.h"
#include "SessionManager.h"

#include "Object.h"
#include "ObjectManager.h"

#include "Packet.h"


CNetIOManager::CNetIOManager() noexcept
{
}

CNetIOManager::~CNetIOManager() noexcept
{
}

void CNetIOManager::netIOProcess(void)
{
    FD_SET ReadSet;
    FD_SET WriteSet;
    int iResult;

    TIMEVAL timeVal;
    timeVal.tv_sec = 0;
    timeVal.tv_usec = 0;

    // select�� �ѹ��� �ִ� 64������ ó�� ����
    // �������� ��� Ŭ���̾�Ʈ�� ���� SOCKET üũ
    UINT32 iCSessionSize = g_clientList.size();

    auto iter = g_clientList.begin();

    static UINT32 netProc_RecvCnt = 0;
    static UINT32 netProc_SendCnt = 0;

    // ����� ������ ������ 64�� �̻��̶��
    while (iCSessionSize >= 64)
    {
        // �� �ʱ�ȭ
        FD_ZERO(&ReadSet);
        FD_ZERO(&WriteSet);

        // begin���� 64���� ���� ����
        for (UINT8 i = 0; i < 64; ++i)
        {
            FD_SET((*iter)->sock, &ReadSet);

            // ���� ���� �����Ͱ� �ִٸ� WriteSet�� ���
            if ((*iter)->sendQ.GetUseSize() > 0)
                FD_SET((*iter)->sock, &WriteSet);

            ++iter;
        }

        // ��ü ������ 64 ����
        iCSessionSize -= 64;

        // select ȣ��
        iResult = select(0, &ReadSet, &WriteSet, NULL, &timeVal);

        // iResult ���� 0 �̻��̶�� ���� ������ / �� �����Ͱ� �ִٴ� ��
        if (iResult > 0)
        {
            auto iter2 = iter;
            for (UINT8 i = 0; i < 64; ++i)
            {
                iter2--;

                if (FD_ISSET((*iter2)->sock, &ReadSet))
                {
                    --iResult;

                    // recv �̺�Ʈ ó��. �޽��� ���� �� �޽��� �б� ���� ó��
                    if ((*iter2)->isAlive)
                    {
                        netProc_Recv((*iter2));
                        ++netProc_RecvCnt;
                    }
                }

                if (FD_ISSET((*iter2)->sock, &WriteSet))
                {
                    --iResult;

                    // send �̺�Ʈ ó��. �޽��� �۽�
                    if ((*iter2)->isAlive)
                    {
                        netProc_Send((*iter2));
                        ++netProc_SendCnt;
                    }
                }
            }
        }
        else if (iResult == SOCKET_ERROR)
        {
            std::cout << "Error : select(), " << WSAGetLastError() << "\n";
            DebugBreak();
        }
    }

    // ����� ������ ������ 64�� ���϶��

    // �� �ʱ�ȭ
    FD_ZERO(&ReadSet);
    FD_ZERO(&WriteSet);

    // ���� �����ŭ for�� ���鼭 FD_SET ����
    for (UINT8 i = 0; i < iCSessionSize; ++i)
    {
        FD_SET((*iter)->sock, &ReadSet);

        // ���� ���� �����Ͱ� �ִٸ� WriteSet�� ���
        if ((*iter)->sendQ.GetUseSize() > 0)
            FD_SET((*iter)->sock, &WriteSet);

        ++iter;
    }


    //==============================================================================================
    // listen ���� ó��
    //==============================================================================================
    CWinSockManager& winSockManager = CWinSockManager::GetInstance();
    SOCKET listenSocket = winSockManager.GetListenSocket();
    FD_SET(listenSocket, &ReadSet);

    // �̷��� listen ������ ó���ϴ� ������ listen ������ blocking�̱� ����


    // select ȣ��
    iResult = select(0, &ReadSet, &WriteSet, NULL, &timeVal);

    // iResult ���� 0 �̻��̶�� ���� ������ / �� �����Ͱ� �ִٴ� ��
    if (iResult > 0)
    {
        // ��α� ť�� ������ �ִٸ�, accept ����
        if (FD_ISSET(listenSocket, &ReadSet))
        {
            --iResult;

            // accept �̺�Ʈ ó��. ���� �� CSession ����
            netProc_Accept();
        }

        // listen ������ ������ ���� ó��
        auto iter2 = iter;
        for (UINT8 i = 0; i < iCSessionSize; ++i)
        {
            iter2--;

            if (FD_ISSET((*iter2)->sock, &ReadSet))
            {
                --iResult;

                // recv �̺�Ʈ ó��. �޽��� ���� �� �޽��� �б� ���� ó��
                if ((*iter2)->isAlive)
                {
                    netProc_Recv((*iter2));
                    ++netProc_RecvCnt;
                }
            }

            if (FD_ISSET((*iter2)->sock, &WriteSet))
            {
                --iResult;

                // send �̺�Ʈ ó��. �޽��� �۽�
                if ((*iter2)->isAlive)
                {
                    netProc_Send((*iter2));
                    ++netProc_SendCnt;
                }
            }
        }
    }
    else if (iResult == SOCKET_ERROR)
    {
        std::cout << "Error : select(), " << WSAGetLastError() << "\n";
        DebugBreak();
    }
}

void CNetIOManager::netProc_Accept(void)
{
    // Ŭ���̾�Ʈ�� �������� �� ����Ǵ� ����
    // ��α� ť�� ������ �Ǿ����� �����ϰ� Accept �õ�
    SOCKET ClientSocket;
    SOCKADDR_IN ClientAddr;

    CWinSockManager& winSockManager = CWinSockManager::GetInstance();
    SOCKET listenSocket = winSockManager.GetListenSocket();

    // accept �õ�
    ClientSocket = winSockManager.Accept(ClientAddr);

    // ������ ���� m_AcceptSessionList�� ����
    CSessionManager& sessionManger = CSessionManager::GetInstance();
    CSession* pSession = createSession(ClientSocket, ClientAddr);

    pSession->pObj = m_callbackAcceptCreate();
    pSession->pObj->m_pSession = pSession;

    g_clientList.push_back(pSession);

    m_callbackAcceptAfter(pSession->pObj);
}


void CNetIOManager::netProc_Send(CSession* pSession)
{
    // �����ۿ� �ִ� ������ �ѹ��� send. 
    int directDeqSize = pSession->sendQ.DirectDequeueSize();
    int useSize = pSession->sendQ.GetUseSize();

    // ����ϴ� �뷮(useSize)�� directDeqSize���� �۰ų� ���� ��� useSize��ŭ�� ������, Ŭ ��� directDeqSize��ŭ ������.

    int retval = send(pSession->sock, pSession->sendQ.GetFrontBufferPtr(), std::min(useSize, directDeqSize), 0);

    // ���⼭ ���� ���� ó��
    if (retval == SOCKET_ERROR)
    {
        int error = WSAGetLastError();

        // �߰��� ������ ���� ����.
        if (error == WSAECONNRESET)
        {
            NotifyClientDisconnected(pSession);
            return;
        }

        // ���⼭ ���� ó�� �۾�
        DebugBreak();
    }

    // ���� send�� �����Ѵٸ�
    if (retval != std::min(useSize, directDeqSize))
    {
        int error = WSAGetLastError();

        // ���⼭ ���� ó�� �۾�
        DebugBreak();
    }

    pSession->sendQ.MoveFront(retval);
}

void CNetIOManager::netProc_Recv(CSession* pSession)
{
    // �ӽ� ���� ����
    char tempRecvBuffer[RINGBUFFER_SIZE];

    // recv �ӽ� ���� ���۷� ȣ��
    int recvRetVal = recv(pSession->sock, tempRecvBuffer, RINGBUFFER_SIZE, 0);

    // recv ��ȯ���� 0�̶�� ���� �����ʿ��� rst�� ���´ٴ� �ǹ��̹Ƿ� disconnect ó���� ������Ѵ�.
    if (recvRetVal == 0)
    {
        // �� �̻� ���� ���� �����Ƿ� break!
        NotifyClientDisconnected(pSession);
        //std::cout << "recv() return 0," << WSAGetLastError() << "\n";
        return;
    }
    // recv ������ ���
    else if (recvRetVal == SOCKET_ERROR)
    {
        int error = WSAGetLastError();

        // ���� ���ۿ� �����Ͱ� �����Ƿ� ���߿� �ٽ� recv �õ�
        if (error == WSAEWOULDBLOCK)
            return;

        // ���� ���ۿ��� ������ �������ٴ�, rst�� �Դٴ� �ǹ�.
        if (error == WSAECONNRESET)
        {
            // �ٸ� Ŭ���̾�Ʈ�鿡 ������ �������ٴ� ���� �˸�.
            NotifyClientDisconnected(pSession);
            return;
        }

        // ���� �ʿ��� closesocket�� �ϸ� recv ����
        if (error == WSAECONNABORTED)
        {
            NotifyClientDisconnected(pSession);
            return;
        }

        // ���� WSAEWOULDBLOCK�� �ƴ� �ٸ� ������� ��¥ ����
        std::cout << "Error : recv(), " << WSAGetLastError() << "\n";
        DebugBreak();
    }

    // �켱 �����͸� ��� ����.
    int recvQEnqRetVal = pSession->recvQ.Enqueue(tempRecvBuffer, recvRetVal);
    if (recvQEnqRetVal != recvRetVal)
    {
        std::cout << "Error : recvQ.Enqueue(), " << WSAGetLastError() << "\n";
        DebugBreak();
    }

    // �Ϸ���Ŷ ó�� ��
    // RecvQ�� �ִ� ��� �ϼ���Ŷ�� �ؼ�/ó��
    while (true)
    {
        // 1. RecvQ�� �ּ����� ����� �ִ��� Ȯ��. ������ [ ��� ������ �̻��� �����Ͱ� �ִ��� Ȯ�� ]�ϴ� ��.
        if (pSession->recvQ.GetUseSize() < sizeof(PACKET_HEADER))
            break;

        // 2. RecvQ���� PACKET_HEADER ���� Peek
        PACKET_HEADER header;
        int headerSize = sizeof(header);
        int retVal = pSession->recvQ.Peek(reinterpret_cast<char*>(&header), headerSize);

        // 3. header�� Code �κ� Ȯ��. CRC Ȯ������ �̻��� ���� ������ disconnect ó��
        if (header.byCode != dfPACKET_CODE)
        {
            NotifyClientDisconnected(pSession);
            break;
        }

        // �߰��� ���ŵ� �������� ũ�Ⱑ �ִ� ��Ŷ ũ�⸦ �Ѿ�� �˻�
        if (header.bySize > dfMAX_PACKET_SIZE)
        {
            DebugBreak();
        }

        // 4. ����� len���� RecvQ�� ������ ������ ��
        if ((header.bySize + sizeof(PACKET_HEADER)) > pSession->recvQ.GetUseSize())
            break;

        // 5. Peek �ߴ� ����� RecvQ���� �����.
        pSession->recvQ.MoveFront(sizeof(PACKET_HEADER));

        // 6. RecvQ���� header�� len ũ�⸸ŭ �ӽ� ��Ŷ ���۸� �̴´�.
        CPacket Packet;
        int recvQDeqRetVal = pSession->recvQ.Dequeue(Packet.GetBufferPtr(), header.bySize);
        Packet.MoveWritePos(header.bySize);

        if(recvQDeqRetVal != header.bySize)
        {
            DebugBreak();
        }

        if (!m_callbackPacketProc(pSession, static_cast<PACKET_TYPE>(header.byType), &Packet))
        {
            NotifyClientDisconnected(pSession);
            break;
        }
    }
}
