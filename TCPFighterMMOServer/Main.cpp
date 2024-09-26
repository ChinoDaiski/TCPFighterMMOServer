

//===================================================
// pch
//===================================================
#include "pch.h"

//===================================================
// �Ŵ��� class
//===================================================
#include "WinSockManager.h" // ������ ����
#include "SessionManager.h"
#include "TimerManager.h"
#include "NetIOManager.h"
#include "ObjectManager.h"
#include "SectorManager.h"

#include "Sector.h"

//===================================================
// ������Ʈ ����
//===================================================
#include "Player.h"
#include "Session.h"
#include "Packet.h"

//===================================================
// ��Ŷ ó�� ����
//===================================================
#include "PacketProc.h"

//===================================================
// ������ ����
//===================================================
#include "Content.h"

//===================================================
// Ű �Է�
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
    // listen ���� �غ�
    //=====================================================================================================================================
    CWinSockManager& winSockManager = CWinSockManager::GetInstance();

    UINT8 options = 0;
    options |= OPTION_NONBLOCKING;

    winSockManager.StartServer(PROTOCOL_TYPE::TCP_IP, dfNETWORK_PORT, options, INADDR_ANY, SOMAXCONN_HINT(65535));

    //=====================================================================================================================================
    // ���� �ð� ����
    //=====================================================================================================================================
    CTimerManager& timerManager = CTimerManager::GetInstance();

    timerManager.InitTimer(25);

    //=====================================================================================================================================
    // recv �� ���������� ó���ϴ� �Լ� ���
    //=====================================================================================================================================
    CNetIOManager& netIOManager = CNetIOManager::GetInstance();
    netIOManager.RegisterPacketProcCallback(PacketProc);        // ��Ŷ�� ó���ϴ� �Լ� ���
    netIOManager.RegisterAcceptCreateProcCallback(CreateAcceptObject);    // accept �� ���ǰ� ����Ǵ� ��ü�� ����� CObject* ��ü�� ��ȯ�ϴ� �Լ��� ���
    netIOManager.RegisterAcceptAfterCreateCallback(LoginAcceptObject);    // accept �� ���ǰ� ����Ǵ� ��ü�� ����� CObject* ��ü�� ��ȯ�ϴ� �Լ��� ���

    //=====================================================================================================================================
    // ������ ����� �� ȣ��� �Լ� ���
    //=====================================================================================================================================
    CSessionManager::RegisterDisconnectCallback(DisconnectSessionProc);

    //=====================================================================================================================================
    // ������ ����� �� ȣ��� �Լ� ���
    //=====================================================================================================================================
    CSectorManager::RegisterCreateSectorObjectCallback(SendCreationPacketBetween);
    CSectorManager::RegisterDeleteSectorObjectCallback(SendDestructionPacketBetween);

    CSectorManager::GetInstance();

    //=====================================================================================================================================
    // ���� ����
    //=====================================================================================================================================
    while (!g_bShutdown)
    {
        try
        {
            // ��Ʈ��ũ I/O ó��
            netIOManager.netIOProcess();

            // ���� ���� ������Ʈ
            if (timerManager.CheckFrame())
                Update();

            // Ű���� �Է��� ���� ������ ������ �� ���
            ServerControl();

            // ����͸� ������ ǥ��, ����, �����ϴ� ��� ���
            Monitor();
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << "\n";

            g_bShutdown = true;
        }
    }

    // ����� ���� ������ �ִ� ���� �����ϰ� DB� ����

    // ������ ���� �Ŵ��� ����
    winSockManager.Cleanup();
}

// ���� ����
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

// Ű���� �Է��� ���� ������ ������ �� ���
void ServerControl(void)
{
    //==========================================================
    // L : ��Ʈ�� Lock     U : ��Ʈ�� Unlock      Q : ���� ����
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

        // ��Ʈ�� ���� ��� Ȯ��
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

// ����͸� ������ ǥ��, ����, �����ϴ� ��� ���
void Monitor(void)
{
    //system("cls");

    //playerPool.PrintUsage();
    //sessionPool.PrintUsage();
    //packetPool.PrintUsage();
}
