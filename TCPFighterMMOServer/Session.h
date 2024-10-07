#pragma once

class CPacket;
class CObject;

#include "RingBuffer.h"

class CSession
{
public:
    explicit CSession();
	~CSession();

public:
    void RegisterObject(CObject** ppObj);

public:
    // ���� ���θ� �Ǻ��ϴ� ����
    bool isAlive;

    // ���� info - ����, ip, port
    USHORT port;
    char IP[16];

    UINT32 SessionID;

    SOCKET sock;
    CRingBuffer recvQ;  // ���ſ� ������
    CRingBuffer sendQ;  // �۽ſ� ������

    CObject* pObj;

    UINT32 recvCnt = 0;
    UINT32 sendCnt = 0;

    UINT32 lastRecvTime = 0;
    UINT32 lastSendTime = 0;

private:
    static UINT32 g_ID;
};