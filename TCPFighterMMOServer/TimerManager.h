#pragma once

#include "Singleton.h"
#include <fstream>	// 파일 입출력을 위한 헤더

class CTimerManager : public SingletonBase<CTimerManager>
{
public:
	explicit CTimerManager() noexcept;
	~CTimerManager() noexcept;

	// 복사 생성자와 대입 연산자를 삭제하여 복사 방지
	CTimerManager(const CTimerManager&) = delete;
	CTimerManager& operator=(const CTimerManager&) = delete;

public:
	// 목표 프레임. 왠만하면 1000의 약수로 설정할 것.
	void InitTimer(DWORD _targetFPS);
	bool CheckFrame(void);

public:
	inline DWORD GetCurrServerTime(void) { return m_currentServerTime; }
	
public:
	void MeasureJitter(void);
	inline DWORD GetJitter(void) { return m_jitter; }
	void PrintJitterStats(void);

private:
	// 프레임 관련
	DWORD m_targetFPS;				// 1초당 목표 프레임
	DWORD m_givenFrameTime;			// 1초당 주어지는 시간 -> 1000 / m_targetFPS
	DWORD m_standardServerTime;		// 서버 로직이 시작될 때 초기화되는 값
	DWORD m_currentServerTime;		// 서버 로직이 시작될 때 초기화되고, 이후에 프레임이 지날 때 마다 m_givenFrameTime 만큼 더함.
	DWORD m_currentMainLogicTime;	// 메인 루프가 돌아갈 때 마다 측정(timeGetTime)하는 값

	UINT16 m_FPS;	// 현재 프레임

private:
	// 지터 측정 관련
	DWORD m_prevNetIOTime;			 // 이전 네트워크 I/O 처리 완료 시간
	DWORD m_currNetIOTime;			 // 현재 네트워크 I/O 처리 완료 시간
	DWORD m_jitter;					 // 지터 값

	// 초당 지터 평균 관련
	DWORD m_minJitter;               // 1초 동안 측정된 지터의 최소값
	DWORD m_maxJitter;               // 1초 동안 측정된 지터의 최대값
	DWORD m_totalJitter;             // 1초 동안 측정된 지터의 총합 (평균 계산을 위해)
	DWORD m_jitterCount;             // 1초 동안 측정된 지터의 개수 (평균 계산을 위해)
};


