

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

//===================================================
// ������Ʈ ����
//===================================================
#include "Player.h"

//===================================================
// ��Ŷ ó�� ����
//===================================================
#include "PacketProc.h"

//===================================================
// ������ ����
//===================================================
#include "Content.h"

bool g_bShutdown = false;

void Update(void);

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

    timerManager.InitTimer(50);

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
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << "\n";

            g_bShutdown = true;
        }
    }

    // ����� ���� ������ �ִ� ���� �����ϰ� DB� ����
}

// ���� ����
void Update(void)
{
    CObjectManager& objectManager = CObjectManager::GetInstance();
    objectManager.Update();
    objectManager.LateUpdate();

    CSessionManager& sessionManager = CSessionManager::GetInstance();
    sessionManager.Update();
}
