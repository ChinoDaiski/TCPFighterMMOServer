#pragma once

#include "Singleton.h"
#include <list>
class CSession;

//==========================================================================================================================================
// Broadcast
//==========================================================================================================================================
void BroadcastData(CSession* excludeCSession, PACKET_HEADER* pPacket, UINT32 dataSize);
void BroadcastData(CSession* excludeCSession, CPacket* pPacket, UINT32 dataSize);
void BroadcastPacket(CSession* excludeCSession, PACKET_HEADER* pHeader, CPacket* pPacket);

//==========================================================================================================================================
// Unicast
//==========================================================================================================================================
void UnicastData(CSession* includeCSession, PACKET_HEADER* pPacket, UINT32 dataSize);
void UnicastData(CSession* includeCSession, CPacket* pPacket, UINT32 dataSize);
void UnicastPacket(CSession* includeCSession, PACKET_HEADER* pHeader, CPacket* pPacket);

//==========================================================================================================================================
// ���� / �Ҹ�
//==========================================================================================================================================
void NotifyClientDisconnected(CSession* disconnectedCSession);
CSession* createSession(SOCKET ClientSocket, SOCKADDR_IN ClientAddr);

extern std::list<CSession*> g_clientList;   // ������ ������ ���ǵ鿡 ���� ����


typedef void(*DisconnectCallback)(CSession* pSession);

class CSessionManager : public SingletonBase<CSessionManager> {
private:
    friend class SingletonBase<CSessionManager>;

public:
    explicit CSessionManager() noexcept;
    ~CSessionManager() noexcept;

    // ���� �����ڿ� ���� �����ڸ� �����Ͽ� ���� ����
    CSessionManager(const CSessionManager&) = delete;
    CSessionManager& operator=(const CSessionManager&) = delete;

public:
    void Update(void);

public:
    friend void BroadcastData(CSession* excludeCSession, PACKET_HEADER* pPacket, UINT32 dataSize);
    friend void BroadcastData(CSession* excludeCSession, CPacket* pPacket, UINT32 dataSize);
    friend void BroadcastPacket(CSession* excludeCSession, PACKET_HEADER* pHeader, CPacket* pPacket);

    friend void UnicastData(CSession* includeCSession, PACKET_HEADER* pPacket, UINT32 dataSize);
    friend void UnicastData(CSession* includeCSession, CPacket* pPacket, UINT32 dataSize);
    friend void UnicastPacket(CSession* includeCSession, PACKET_HEADER* pHeader, CPacket* pPacket);

public:
    static void RegisterDisconnectCallback(DisconnectCallback callback) { m_callbackDisconnect = callback; }

private:
    static DisconnectCallback m_callbackDisconnect;
};
