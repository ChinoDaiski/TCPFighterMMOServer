

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
#include "SectorManager.h"

#include "Sector.h"

//===================================================
// 오브젝트 관련
//===================================================
#include "Player.h"
#include "Session.h"
#include "Packet.h"

//===================================================
// 패킷 처리 관련
//===================================================
#include "PacketProc.h"

//===================================================
// 콘텐츠 관련
//===================================================
#include "Content.h"

//===================================================
// 키 입력
//===================================================
#include <conio.h>


#include "MemoryPoolManager.h"


bool g_bShutdown = false;

void Update(void);
void ServerControl(void);
void Monitor(void);

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

    timerManager.InitTimer(25);

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
    // 세션이 종료될 때 호출될 함수 등록
    //=====================================================================================================================================
    CSectorManager::RegisterCreateSectorObjectCallback(SendCreationPacketBetween);
    CSectorManager::RegisterDeleteSectorObjectCallback(SendDestructionPacketBetween);

    CSectorManager::GetInstance();

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

            // 키보드 입력을 통해 게임을 제어할 시 사용
            ServerControl();

            // 모니터링 정보를 표시, 저장, 전송하는 경우 사용
            Monitor();
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << "\n";

            g_bShutdown = true;
        }
    }

    // 종료시 현재 서버에 있는 정보 안전하게 DB등에 저장

    // 윈도우 소켓 매니저 정리
    winSockManager.Cleanup();
}

// 메인 로직
void Update(void)
{
    CObjectManager& objectManager = CObjectManager::GetInstance();
    CSessionManager& sessionManager = CSessionManager::GetInstance();

    objectManager.Update();
    objectManager.LateUpdate();

    sessionManager.Update();
}

bool PressKey(WCHAR checkKey, WCHAR input)
{
    return towlower(checkKey) == towlower(input);
}

// 키보드 입력을 통해 게임을 제어할 시 사용
void ServerControl(void)
{
    //==========================================================
    // L : 컨트롤 Lock     U : 컨트롤 Unlock      Q : 서버 종료
    //==========================================================

    static bool bControlMode = false;

    if (_kbhit())
    {
        WCHAR ControlKey = _getch();

        if (PressKey(ControlKey, L'U'))
        {
            if (!bControlMode)
            {
                std::wcout << L"Control Mode : Press Q - Quit\n";
                std::wcout << L"Control Mode : Press L - Key Lock\n";

                bControlMode = true;
            }
        }

        if (PressKey(ControlKey, L'L'))
        {
            if (bControlMode)
            {
                std::wcout << L"Control Locked. Press U to Unlock Control\n";

                bControlMode = false;
            }
        }

        // 컨트롤 중인 경우 확인
        if (bControlMode)
        {
            if (PressKey(ControlKey, L'Q'))
            {
                g_bShutdown = true;

                std::wcout << L"Server Shutdown\n";
            }
        }
    }
}

// 모니터링 정보를 표시, 저장, 전송하는 경우 사용
void Monitor(void)
{
    //system("cls");

    //playerPool.PrintUsage();
    //sessionPool.PrintUsage();
    //packetPool.PrintUsage();
}
