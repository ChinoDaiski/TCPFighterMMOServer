
#include "pch.h"
#include "MemoryPoolManager.h"
#include "Player.h"
#include "Session.h"
#include "Packet.h"
#include "TimerManager.h"

MemoryPool<CPlayer, false> playerPool; //(dfEXPECTED_ACTIVE_USERS);
MemoryPool<CSession, false> sessionPool;// (dfEXPECTED_ACTIVE_USERS);
MemoryPool<CPacket, false> packetPool;// (100);