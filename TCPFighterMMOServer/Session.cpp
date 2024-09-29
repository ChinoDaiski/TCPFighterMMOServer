
#include "pch.h"
#include "Session.h"
#include "Packet.h"
#include "WinSockManager.h"
#include "Object.h"
#include "RingBuffer.h"

UINT32 CSession::g_ID = 0;

CSession::CSession()
{
    SessionID = g_ID;
    ++g_ID;
}

CSession::~CSession()
{
}

void CSession::RegisterObject(CObject** ppObj)
{
    pObj = *ppObj;
}
