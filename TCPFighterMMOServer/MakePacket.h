#pragma once

class CSession;
class CSector;

void SC_CREATE_MY_CHARACTER_FOR_All(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y, UINT8 HP);
void SC_CREATE_MY_CHARACTER_FOR_SINGLE(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y, UINT8 HP);
void SC_CREATE_MY_CHARACTER_AROUND(CSession* pSession, CSector* pSector, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y, UINT8 HP);

void SC_CREATE_OTHER_CHARACTER_FOR_All(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y, UINT8 HP);
void SC_CREATE_OTHER_CHARACTER_FOR_SINGLE(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y, UINT8 HP);
void SC_CREATE_OTHER_CHARACTER_AROUND(CSession* pSession, CSector* pSector, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y, UINT8 HP);

void SC_DELETE_CHARACTER_FOR_All(CSession* pSession, UINT32 ID);
void SC_DELETE_CHARACTER_FOR_SINGLE(CSession* pSession, UINT32 ID);
void SC_DELETE_CHARACTER_FOR_AROUND(CSession* pSession, CSector* pSector, UINT32 ID);

void SC_MOVE_START_FOR_All(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y);
void SC_MOVE_START_FOR_SINGLE(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y);
void SC_MOVE_START_FOR_AROUND(CSession* pSession, CSector* pSector, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y);

void SC_MOVE_STOP_FOR_All(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y);
void SC_MOVE_STOP_FOR_SINGLE(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y);
void SC_MOVE_STOP_FOR_AROUND(CSession* pSession, CSector* pSector, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y);

void SC_ATTACK1_FOR_All(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y);
void SC_ATTACK1_FOR_SINGLE(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y);
void SC_ATTACK1_FOR_AROUND(CSession* pSession, CSector* pSector, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y);

void SC_ATTACK2_FOR_All(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y);
void SC_ATTACK2_FOR_SINGLE(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y);
void SC_ATTACK2_FOR_AROUND(CSession* pSession, CSector* pSector, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y);

void SC_ATTACK3_FOR_All(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y);
void SC_ATTACK3_FOR_SINGLE(CSession* pSession, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y);
void SC_ATTACK3_FOR_AROUND(CSession* pSession, CSector* pSector, UINT32 ID, UINT8 Direction, UINT16 X, UINT16 Y);

void SC_DAMAGE_FOR_All(CSession* pSession, UINT32 AttackID, UINT32 DamageID, UINT8 DamageHP);
void SC_DAMAGE_FOR_SINGLE(CSession* pSession, UINT32 AttackID, UINT32 DamageID, UINT8 DamageHP);
void SC_DAMAGE_FOR_AROUND(CSession* pSession, CSector* pSector, UINT32 AttackID, UINT32 DamageID, UINT8 DamageHP);

void SC_ECHO_FOR_All(CSession* pSession, UINT32 time);
void SC_ECHO_FOR_SINGLE(CSession* pSession, UINT32 time);
void SC_ECHO_FOR_AROUND(CSession* pSession, CSector* pSector, UINT32 time);

void SC_SYNC_FOR_All(CSession* pSession, UINT32 ID, UINT16 X, UINT16 Y);
void SC_SYNC_FOR_SINGLE(CSession* pSession, UINT32 ID, UINT16 X, UINT16 Y);
void SC_SYNC_FOR_AROUND(CSession* pSession, CSector* pSector, UINT32 ID, UINT16 X, UINT16 Y);