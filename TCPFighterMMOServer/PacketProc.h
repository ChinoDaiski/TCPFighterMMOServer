#pragma once

#include "Session.h"

extern int g_iSyncCount;

class CObject;
class CSession;

bool PacketProc(CSession* pSession, PACKET_TYPE packetType, CPacket* pPacket);

bool CS_MOVE_START(CSession* pSession, UINT8 direction, UINT16 x, UINT16 y);
bool CS_MOVE_STOP(CSession* pSession, UINT8 direction, UINT16 x, UINT16 y);
bool CS_ATTACK1(CSession* pSession, UINT8 direction, UINT16 x, UINT16 y);
bool CS_ATTACK2(CSession* pSession, UINT8 direction, UINT16 x, UINT16 y);
bool CS_ATTACK3(CSession* pSession, UINT8 direction, UINT16 x, UINT16 y);
bool CS_ECHO(CSession* pSession, UINT32 time);
bool CS_SYNC(CSession* pSession, UINT16 x, UINT16 y);

void DisconnectSessionProc(CSession* pSession);
