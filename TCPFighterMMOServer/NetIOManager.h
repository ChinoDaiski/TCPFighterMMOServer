#pragma once

#include "Singleton.h"

class CSession;
class CPacket;
class CObject;

typedef CObject * (*FunctionPtr)();

class CNetIOManager : public SingletonBase<CNetIOManager> {
private:
    friend class SingletonBase<CNetIOManager>;

public: 
    using PacketProcCallback = bool(*)(CSession*, PACKET_TYPE, CPacket*);
    using AcceptCreateCallback = CObject * (*)();
    using AcceptAfterCallback = void(*)(CObject*);

public:
    explicit CNetIOManager() noexcept;
    ~CNetIOManager() noexcept;

    // 복사 생성자와 대입 연산자를 삭제하여 복사 방지
    CNetIOManager(const CNetIOManager&) = delete;
    CNetIOManager& operator=(const CNetIOManager&) = delete;

public:
    void netIOProcess(void);
    void netProc_Accept(void);
    void netProc_Send(CSession* pSession);
    void netProc_Recv(CSession* pSession);

public:
    void RegisterPacketProcCallback(PacketProcCallback callback) { m_callbackPacketProc = callback; }
    void RegisterAcceptAfterCreateCallback(AcceptAfterCallback callback) { m_callbackAcceptAfter = callback; }
    void RegisterAcceptCreateProcCallback(AcceptCreateCallback callback) { m_callbackAcceptCreate = callback; }

private:
    PacketProcCallback m_callbackPacketProc;
    AcceptCreateCallback m_callbackAcceptCreate;
    AcceptAfterCallback m_callbackAcceptAfter;
};