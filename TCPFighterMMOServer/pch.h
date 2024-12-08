#pragma once

#define NOMINMAX

#include <iostream>
#include <stdexcept>

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#define _WINSOCKAPI_	// windows.h와 winsock2를 같이 사용할 경우 생기는 문제를 해결하기 위해 사용하는 define문
#include <Windows.h>
#pragma comment(lib, "Winmm.lib")

//===================================================
// 자료구조
//===================================================
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
//===================================================
#include <string>


// 네트워크 관련

//===================================================
// 세션 정보
//===================================================
#include "Session.h"
//===================================================



// 나중에 pch에서 제외할 예정

//===================================================
// 프로토콜 정보
//===================================================
#include "Protocol.h"
//===================================================



//===================================================
// Dump 관련
//===================================================
#include "CrashDump.h"