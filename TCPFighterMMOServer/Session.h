#pragma once

class CPacket;
class CObject;

#include "RingBuffer.h"
#include "CircularQueue.h"

class CSession
{
public:
    explicit CSession();
	~CSession();

public:
    void RegisterObject(CObject** ppObj);

public:
    // 삭제 여부를 판별하는 변수
    bool isAlive;

    // 세션 info - 소켓, ip, port
    USHORT port;
    char IP[16];

    UINT32 SessionID;

    SOCKET sock;
    CRingBuffer recvQ;  // 수신용 링버퍼
    CRingBuffer sendQ;  // 송신용 링버퍼

    CObject* pObj;

    UINT32 recvCnt = 0;
    UINT32 sendCnt = 0;

    UINT32 lastRecvTime = 0;
    UINT32 lastSendTime = 0;

    CircularQueue<std::tuple<LONG, LONG, UINT8, UINT16, UINT16, UINT8>> debugLogQueue;

private:
    static UINT32 g_ID;
};
