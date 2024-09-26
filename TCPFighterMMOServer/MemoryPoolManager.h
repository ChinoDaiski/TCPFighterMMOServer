#pragma once

#include "MemoryPool.h"

class CPlayer;
class CSession;
class CPacket;

extern MemoryPool<CPlayer, false> playerPool;
extern MemoryPool<CSession, false> sessionPool; 
extern MemoryPool<CPacket, false> packetPool;