#include "pch.h"
#include "NetIOManager.h"

#include "WinSockManager.h"

#include "Session.h"
#include "SessionManager.h"

#include "Object.h"
#include "ObjectManager.h"

#include "Packet.h"

#include "TimerManager.h"

#include "Player.h"

CTimerManager& timerManager = CTimerManager::GetInstance();

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

    // select는 한번에 최대 64개까지 처리 가능
    // 접속중인 모든 클라이언트에 대해 SOCKET 체크
    UINT32 iCSessionSize = g_SessionHashMap.size();

    auto iter = g_SessionHashMap.begin();

    // 연결된 세션의 갯수가 64개 이상이라면
    while (iCSessionSize >= 64)
    {
        // 셋 초기화
        FD_ZERO(&ReadSet);
        FD_ZERO(&WriteSet);

        // begin부터 64개를 돌며 실행
        for (UINT8 i = 0; i < 64; ++i)
        {
            FD_SET((*iter).second->sock, &ReadSet);

            // 만약 보낼 데이터가 있다면 WriteSet에 등록
            if ((*iter).second->sendQ.GetUseSize() > 0)
                FD_SET((*iter).second->sock, &WriteSet);

            ++iter;
        }

        // 전체 사이즈 64 빼기
        iCSessionSize -= 64;

        // select 호출
        iResult = select(0, &ReadSet, &WriteSet, NULL, &timeVal);

        // iResult 값이 0 이상이라면 읽을 데이터 / 쓸 데이터가 있다는 뜻
        if (iResult >= 0)
        {
            auto iter2 = iter;
            for (UINT8 i = 0; i < 64; ++i)
            {
                iter2--;

                if (FD_ISSET((*iter2).second->sock, &ReadSet))
                {
                    --iResult;

                    // recv 이벤트 처리. 메시지 수신 및 메시지 분기 로직 처리
                    // 같은 프레임에 죽었다고 하더라도 이미 보낸 것 까지는 처리해주기로 했다. 아니면 나중에 접속했다는 이유로 같은 프레임에 손해를 보기 때문이다.
                    // 만약 소켓 에러로 인한 isAlive = false 처리라면 recv하면서 에러가 감지되며 return 될 것이다. 그러니 이렇게 진행.
                    if ((*iter2).second->isAlive)
                    {
                        (*iter2).second->recvCnt++;
                        (*iter2).second->lastRecvTime = timerManager.GetCurrServerTime();

                        netProc_Recv((*iter2).second);
                    }
                }

                if (FD_ISSET((*iter2).second->sock, &WriteSet))
                {
                    --iResult;

                    // send 이벤트 처리. 메시지 송신
                    if ((*iter2).second->isAlive)
                    {
                        (*iter2).second->sendCnt++;
                        (*iter2).second->lastSendTime = timerManager.GetCurrServerTime();

                        netProc_Send((*iter2).second);
                    }
                }

                if (iResult == 0)
                    break;
            }
        }
        else if (iResult == SOCKET_ERROR)
        {
            std::cout << "Error : select(), " << WSAGetLastError() << "\n";
            DebugBreak();
        }
    }

    // 연결된 세션의 갯수가 64개 미만이라면

    // 셋 초기화
    FD_ZERO(&ReadSet);
    FD_ZERO(&WriteSet); 

    // 남은 사이즈만큼 for문 돌면서 FD_SET 실행
    for (UINT8 i = 0; i < iCSessionSize; ++i)
    {
        FD_SET((*iter).second->sock, &ReadSet);

        // 만약 보낼 데이터가 있다면 WriteSet에 등록
        if ((*iter).second->sendQ.GetUseSize() > 0)
            FD_SET((*iter).second->sock, &WriteSet);

        ++iter;
    }


    //==============================================================================================
    // listen 소켓 처리
    //==============================================================================================
    CWinSockManager& winSockManager = CWinSockManager::GetInstance();
    SOCKET listenSocket = winSockManager.GetListenSocket();
    FD_SET(listenSocket, &ReadSet);

    // 이렇게 listen 소켓을 처리하는 이유는 listen 소켓이 blocking이기 때문


    // select 호출
    iResult = select(0, &ReadSet, &WriteSet, NULL, &timeVal);

    auto iter2 = iter;
    // iResult 값이 0 이상이라면 읽을 데이터 / 쓸 데이터가 있다는 뜻
    if (iResult >= 0)
    {
        // listen 소켓을 제외한 세션 처리
        for (UINT8 i = 0; i < iCSessionSize; ++i)
        {
            iter2--;

            if (FD_ISSET((*iter2).second->sock, &ReadSet))
            {
                --iResult;

                // recv 이벤트 처리. 메시지 수신 및 메시지 분기 로직 처리
                // 같은 프레임에 죽었다고 하더라도 이미 보낸 것 까지는 처리해주기로 했다. 아니면 나중에 접속했다는 이유로 같은 프레임에 손해를 보기 때문이다.
                // 만약 소켓 에러로 인한 isAlive = false 처리라면 recv하면서 에러가 감지되며 return 될 것이다. 그러니 이렇게 진행.
                if ((*iter2).second->isAlive)
                {
                    (*iter2).second->recvCnt++;
                    (*iter2).second->lastRecvTime = timerManager.GetCurrServerTime();

                    netProc_Recv((*iter2).second);
                }
            }

            if (FD_ISSET((*iter2).second->sock, &WriteSet))
            {
                --iResult;

                // send 이벤트 처리. 메시지 송신
                if ((*iter2).second->isAlive)
                {
                    (*iter2).second->sendCnt++;
                    (*iter2).second->lastSendTime = timerManager.GetCurrServerTime();

                    netProc_Send((*iter2).second);
                }
            }
        }

        // 백로그 큐에 소켓이 있다면, accept 진행
        if (FD_ISSET(listenSocket, &ReadSet))
        {
            --iResult;

            // accept 이벤트 처리. 접속 및 CSession 생성
            netProc_Accept();
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
    // 클라이언트가 접속했을 때 진행되는 과정
    // 백로그 큐에 접속이 되었음을 감지하고 Accept 시도
    SOCKET acceptedSocket;
    SOCKADDR_IN ClientAddr;

    CWinSockManager& winSockManager = CWinSockManager::GetInstance();
    SOCKET listenSocket = winSockManager.GetListenSocket();

    while (true)
    {
        // accept 시도
        acceptedSocket = winSockManager.Accept(ClientAddr);

        if (WSAGetLastError() == WSAEWOULDBLOCK || acceptedSocket == INVALID_SOCKET)
            break;

        // 세션 생성 후 세션 매니저에서 관리
        CSessionManager& sessionManger = CSessionManager::GetInstance();
        CSession* pSession = createSession(acceptedSocket, ClientAddr);
        g_SessionHashMap.emplace(acceptedSocket, pSession);

        // 세션과 연결된 오브젝트 생성
        CObject* pObj = m_callbackAcceptCreate();

        // 오브젝트에 세션 정보 등록
        pObj->m_pSession = pSession;
        pSession->pObj = pObj;

        // 오브젝트 생성 이후 처리되어야할 함수 호출
        m_callbackAcceptAfter(pObj);

        // 생성된 오브젝트 오브젝트 매니저에서 관리
        static CObjectManager& ObjectManager = CObjectManager::GetInstance();
        ObjectManager.RegisterObject(pObj);
    }
}


void CNetIOManager::netProc_Send(CSession* pSession)
{
    // 링버퍼에 있는 데이터 한번에 send. 
    int directDeqSize = pSession->sendQ.DirectDequeueSize();
    int useSize = pSession->sendQ.GetUseSize();

    // 사용하는 용량(useSize)이 directDeqSize보다 작거나 같을 경우 useSize만큼만 보내고, 클 경우 directDeqSize만큼 보낸다.

    int retval = send(pSession->sock, pSession->sendQ.GetFrontBufferPtr(), std::min(useSize, directDeqSize), 0);

    // 여기서 소켓 에러 처리
    if (retval == SOCKET_ERROR)
    {
        int error = WSAGetLastError();

        // 중간에 강제로 연결 끊김.
        if (error == WSAECONNRESET)
        {
            pSession->pObj->m_bDead = true;
            NotifyClientDisconnected(pSession);
            return;
        }

        // 여기서 예외 처리 작업
        DebugBreak();
    }

    // 만약 send가 실패한다면
    if (retval != std::min(useSize, directDeqSize))
    {
        int error = WSAGetLastError();

        // 여기서 예외 처리 작업
        DebugBreak();
    }

    pSession->sendQ.MoveFront(retval);
}

void CNetIOManager::netProc_Recv(CSession* pSession)
{
    // 임시 수신 버퍼
    char tempRecvBuffer[RINGBUFFER_SIZE];

    // recv 임시 수신 버퍼로 호출
    int recvRetVal = recv(pSession->sock, tempRecvBuffer, RINGBUFFER_SIZE, 0);

    // recv 반환값이 0이라는 것은 상대방쪽에서 rst를 보냈다는 의미이므로 disconnect 처리를 해줘야한다.
    if (recvRetVal == 0)
    {
        // 더 이상 받을 값이 없으므로 break!
        NotifyClientDisconnected(pSession);
        //std::cout << "recv() return 0," << WSAGetLastError() << "\n";
        return;
    }
    // recv 에러일 경우
    else if (recvRetVal == SOCKET_ERROR)
    {
        int error = WSAGetLastError();

        // 수신 버퍼에 데이터가 없으므로 나중에 다시 recv 시도
        if (error == WSAEWOULDBLOCK)
            return;

        // 수신 버퍼와의 연결이 끊어진다는, rst가 왔다는 의미.
        if (error == WSAECONNRESET)
        {
            pSession->pObj->m_bDead = true;
            // 다른 클라이언트들에 연결이 끊어졌다는 것을 알림.
            NotifyClientDisconnected(pSession);
            return;
        }

        // 상대방 쪽에서 closesocket을 하면 recv 실패
        if (error == WSAECONNABORTED)
        {
            pSession->pObj->m_bDead = true;
            NotifyClientDisconnected(pSession);
            return;
        }

        // 만약 WSAEWOULDBLOCK이 아닌 다른 에러라면 진짜 문제
        std::cout << "Error : recv(), " << WSAGetLastError() << "\n";
        DebugBreak();
    }

    // 우선 데이터를 모두 넣음.
    int recvQEnqRetVal = pSession->recvQ.Enqueue(tempRecvBuffer, recvRetVal);
    if (recvQEnqRetVal != recvRetVal)
    {
        std::cout << "Error : recvQ.Enqueue(), " << WSAGetLastError() << "\n";
        DebugBreak();
    }

    // 완료패킷 처리 부
    // RecvQ에 있는 모든 완성패킷을 해석/처리
    while (true)
    {
        // 1. RecvQ에 최소한의 사이즈가 있는지 확인. 조건은 [ 헤더 사이즈 이상의 데이터가 있는지 확인 ]하는 것.
        if (pSession->recvQ.GetUseSize() < sizeof(PACKET_HEADER))
            break;

        // 2. RecvQ에서 PACKET_HEADER 정보 Peek
        PACKET_HEADER header;
        int headerSize = sizeof(header);
        int retVal = pSession->recvQ.Peek(reinterpret_cast<char*>(&header), headerSize);

        // 3. header의 Code 부분 확인. CRC 확인으로 이상한 값이 있으면 disconnect 처리
        if (header.byCode != dfNETWORK_PACKET_CODE)
        {
            NotifyClientDisconnected(pSession);
            break;
        }

        // 추가로 수신된 데이터의 크기가 최대 패킷 크기를 넘어서면 검사
        if (header.bySize > dfMAX_PACKET_SIZE)
        {
            DebugBreak();
        }

        // 4. 헤더의 len값과 RecvQ의 데이터 사이즈 비교
        if ((header.bySize + sizeof(PACKET_HEADER)) > pSession->recvQ.GetUseSize())
            break;

        // 5. Peek 했던 헤더를 RecvQ에서 지운다.
        pSession->recvQ.MoveFront(sizeof(PACKET_HEADER));

        // 6. RecvQ에서 header의 len 크기만큼 임시 패킷 버퍼를 뽑는다.
        CPacket Packet;
        int recvQDeqRetVal = pSession->recvQ.Dequeue(Packet.GetBufferPtr(), header.bySize);
        Packet.MoveWritePos(header.bySize);

        if(recvQDeqRetVal != header.bySize)
        {
            DebugBreak();
        }

        if (!m_callbackPacketProc(pSession, static_cast<PACKET_TYPE>(header.byType), &Packet))
        {
            pSession->pObj->m_bDead = true;
            NotifyClientDisconnected(pSession);
            break;
        }
        else
            pSession->pObj->SetCurTimeout();
    }
}
