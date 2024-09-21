

//===================================================
// pch
//===================================================
#include "pch.h"

//===================================================
// 매니저 class
//===================================================
#include "WinSockManager.h" // 윈도우 소켓
#include "SessionManager.h"
#include "TimerManager.h"
#include "NetIOManager.h"
#include "ObjectManager.h"

//===================================================
// 오브젝트 관련
//===================================================
#include "Player.h"

//===================================================
// 패킷 처리 관련
//===================================================
#include "PacketProc.h"

//===================================================
// 콘텐츠 관련
//===================================================
#include "Content.h"

bool g_bShutdown = false;

void Update(void);

int main()
{
    //=====================================================================================================================================
    // listen 소켓 준비
    //=====================================================================================================================================
    CWinSockManager& winSockManager = CWinSockManager::GetInstance();

    UINT8 options = 0;
    options |= OPTION_NONBLOCKING;

    winSockManager.StartServer(PROTOCOL_TYPE::TCP_IP, dfNETWORK_PORT, options, INADDR_ANY, SOMAXCONN_HINT(65535));

    //=====================================================================================================================================
    // 서버 시간 설정
    //=====================================================================================================================================
    CTimerManager& timerManager = CTimerManager::GetInstance();

    timerManager.InitTimer(50);

    //=====================================================================================================================================
    // recv 한 프로토콜을 처리하는 함수 등록
    //=====================================================================================================================================
    CNetIOManager& netIOManager = CNetIOManager::GetInstance();
    netIOManager.RegisterPacketProcCallback(PacketProc);        // 패킷을 처리하는 함수 등록
    netIOManager.RegisterAcceptCreateProcCallback(CreateAcceptObject);    // accept 시 세션과 연결되는 객체를 만드는 CObject* 객체를 반환하는 함수를 등록
    netIOManager.RegisterAcceptAfterCreateCallback(LoginAcceptObject);    // accept 시 세션과 연결되는 객체를 만드는 CObject* 객체를 반환하는 함수를 등록

    //=====================================================================================================================================
    // 세션이 종료될 때 호출될 함수 등록
    //=====================================================================================================================================
    CSessionManager::RegisterDisconnectCallback(DisconnectSessionProc);

    //=====================================================================================================================================
    // 메인 로직
    //=====================================================================================================================================
    while (!g_bShutdown)
    {
        try
        {
            // 네트워크 I/O 처리
            netIOManager.netIOProcess();

            // 게임 로직 업데이트
            if (timerManager.CheckFrame())
                Update();
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << "\n";

            g_bShutdown = true;
        }
    }

    // 종료시 현재 서버에 있는 정보 안전하게 DB등에 저장
}

// 메인 로직
void Update(void)
{
    CObjectManager& objectManager = CObjectManager::GetInstance();
    objectManager.Update();
    objectManager.LateUpdate();

    CSessionManager& sessionManager = CSessionManager::GetInstance();
    sessionManager.Update();
}
