#include "pch.h"
#include "MakePacket.h"
#include "SessionManager.h"
#include "Packet.h"
#include "Player.h"
#include "Sector.h"

#include "MemoryPoolManager.h"

void SC_CREATE_MY_CHARACTER_FOR_All(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y, UINT8 HP)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;
    *Packet << HP;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_CREATE_MY_CHARACTER;

    BroadcastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_CREATE_MY_CHARACTER_FOR_SINGLE(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y, UINT8 HP)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;
    *Packet << HP;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_CREATE_MY_CHARACTER;

    UnicastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_CREATE_MY_CHARACTER_AROUND(CSession* pSession, CSector* pSector, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y, UINT8 HP)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;
    *Packet << HP;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_CREATE_MY_CHARACTER;

    for (auto& Sector : pSector->GetAroundSectorList())
    {
        for (auto& Object : Sector->GetSectorObjectMap())
        {
            if (pSession == Object.second->m_pSession)
                continue;

            UnicastPacket(Object.second->m_pSession, &header, Packet);
        }
    }

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_CREATE_OTHER_CHARACTER_FOR_All(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y, UINT8 HP)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;
    *Packet << HP;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;

    BroadcastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_CREATE_OTHER_CHARACTER_FOR_SINGLE(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y, UINT8 HP)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;
    *Packet << HP;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;

    UnicastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_CREATE_OTHER_CHARACTER_AROUND(CSession* pSession, CSector* pSector, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y, UINT8 HP)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;
    *Packet << HP;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;

    for (auto& Sector : pSector->GetAroundSectorList())
    {
        for (auto& Object : Sector->GetSectorObjectMap())
        {
            if (pSession == Object.second->m_pSession)
                continue;

            UnicastPacket(Object.second->m_pSession, &header, Packet);
        }
    }

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_DELETE_CHARACTER_FOR_All(CSession* pSession, UINT32 ID)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_DELETE_CHARACTER;

    BroadcastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_DELETE_CHARACTER_FOR_SINGLE(CSession* pSession, UINT32 ID)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_DELETE_CHARACTER;

    UnicastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_DELETE_CHARACTER_FOR_AROUND(CSession* pSession, CSector* pSector, UINT32 ID)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_DELETE_CHARACTER;

    for (auto& Sector : pSector->GetAroundSectorList())
    {
        for (auto& Object : Sector->GetSectorObjectMap())
        {
            if (pSession == Object.second->m_pSession)
                continue;

            UnicastPacket(Object.second->m_pSession, &header, Packet);
        }
    }

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_MOVE_START_FOR_All(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_MOVE_START;

    BroadcastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_MOVE_START_FOR_AROUND(CSession* pSession, CSector* pSector, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_MOVE_START;

    for (auto& Sector : pSector->GetAroundSectorList())
    {
        for (auto& Object : Sector->GetSectorObjectMap())
        {
            if (pSession == Object.second->m_pSession)
                continue;

            UnicastPacket(Object.second->m_pSession, &header, Packet);
        }
    }

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_MOVE_START_FOR_SINGLE(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_MOVE_START;

    UnicastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_MOVE_STOP_FOR_All(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_MOVE_STOP;

    BroadcastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_MOVE_STOP_FOR_SINGLE(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_MOVE_STOP;

    UnicastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_MOVE_STOP_FOR_AROUND(CSession* pSession, CSector* pSector, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_MOVE_STOP;

    for (auto& Sector : pSector->GetAroundSectorList())
    {
        for (auto& Object : Sector->GetSectorObjectMap())
        {
            if (pSession == Object.second->m_pSession)
                continue;

            UnicastPacket(Object.second->m_pSession, &header, Packet);
        }
    }

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_ATTACK1_FOR_All(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_ATTACK1;

    BroadcastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_ATTACK1_FOR_SINGLE(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_ATTACK1;

    UnicastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_ATTACK1_FOR_AROUND(CSession* pSession, CSector* pSector, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_ATTACK1;

    for (auto& Sector : pSector->GetAroundSectorList())
    {
        for (auto& Object : Sector->GetSectorObjectMap())
        {
            if (pSession == Object.second->m_pSession)
                continue;

            UnicastPacket(Object.second->m_pSession, &header, Packet);
        }
    }

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_ATTACK2_FOR_All(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_ATTACK2;

    BroadcastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_ATTACK2_FOR_SINGLE(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_ATTACK2;

    UnicastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_ATTACK2_FOR_AROUND(CSession* pSession, CSector* pSector, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_ATTACK2;

    for (auto& Sector : pSector->GetAroundSectorList())
    {
        for (auto& Object : Sector->GetSectorObjectMap())
        {
            if (pSession == Object.second->m_pSession)
                continue;

            UnicastPacket(Object.second->m_pSession, &header, Packet);
        }
    }

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_ATTACK3_FOR_All(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_ATTACK3;

    BroadcastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_ATTACK3_FOR_SINGLE(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_ATTACK3;

    UnicastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_ATTACK3_FOR_AROUND(CSession* pSession, CSector* pSector, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << Direction;
    *Packet << X;
    *Packet << Y;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_ATTACK3;

    for (auto& Sector : pSector->GetAroundSectorList())
    {
        for (auto& Object : Sector->GetSectorObjectMap())
        {
            if (pSession == Object.second->m_pSession)
                continue;

            UnicastPacket(Object.second->m_pSession, &header, Packet);
        }
    }

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_DAMAGE_FOR_All(CSession* pSession, UINT32 AttackID, UINT32 DamageID, UINT8 DamageHP)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << AttackID;
    *Packet << DamageID;
    *Packet << DamageHP;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_DAMAGE;

    BroadcastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_DAMAGE_FOR_SINGLE(CSession* pSession, UINT32 AttackID, UINT32 DamageID, UINT8 DamageHP)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << AttackID;
    *Packet << DamageID;
    *Packet << DamageHP;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_DAMAGE;

    BroadcastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_DAMAGE_FOR_AROUND(CSession* pSession, CSector* pSector, UINT32 AttackID, UINT32 DamageID, UINT8 DamageHP)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << AttackID;
    *Packet << DamageID;
    *Packet << DamageHP;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_DAMAGE;

    for (auto& Sector : pSector->GetAroundSectorList())
    {
        for (auto& Object : Sector->GetSectorObjectMap())
        {
            if (pSession == Object.second->m_pSession)
                continue;

            UnicastPacket(Object.second->m_pSession, &header, Packet);
        }
    }

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_ECHO_FOR_All(CSession* pSession, UINT32 time)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << time;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_ECHO;

    UnicastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_ECHO_FOR_SINGLE(CSession* pSession, UINT32 time)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << time;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_ECHO;

    UnicastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_ECHO_FOR_AROUND(CSession* pSession, CSector* pSector, UINT32 time)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << time;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_ECHO;

    for (auto& Sector : pSector->GetAroundSectorList())
    {
        for (auto& Object : Sector->GetSectorObjectMap())
        {
            if (pSession == Object.second->m_pSession)
                continue;

            UnicastPacket(Object.second->m_pSession, &header, Packet);
        }
    }

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_SYNC_FOR_All(CSession* pSession, UINT32 ID, UINT16 X, UINT16 Y)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << X;
    *Packet << Y;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_SYNC;

    BroadcastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_SYNC_FOR_SINGLE(CSession* pSession, UINT32 ID, UINT16 X, UINT16 Y)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << X;
    *Packet << Y;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_SYNC;

    UnicastPacket(pSession, &header, Packet);

    Packet->Clear();
    packetPool.Free(Packet);
}

void SC_SYNC_FOR_AROUND(CSession* pSession, CSector* pSector, UINT32 ID, UINT16 X, UINT16 Y)
{
    PACKET_HEADER header;
    CPacket* Packet = packetPool.Alloc();

    *Packet << ID;
    *Packet << X;
    *Packet << Y;

    header.byCode = dfNETWORK_PACKET_CODE;
    header.bySize = Packet->GetDataSize();
    header.byType = dfPACKET_SC_SYNC;

    for (auto& Sector : pSector->GetAroundSectorList())
    {
        for (auto& Object : Sector->GetSectorObjectMap())
        {
            if (pSession == Object.second->m_pSession)
                continue;

            UnicastPacket(Object.second->m_pSession, &header, Packet);
        }
    }

    Packet->Clear();
    packetPool.Free(Packet);
}
