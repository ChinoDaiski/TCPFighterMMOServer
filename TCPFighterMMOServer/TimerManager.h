#pragma once

#include "Singleton.h"

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

private:
	DWORD m_targetFPS;				// 1초당 목표 프레임
	DWORD m_givenFrameTime;			// 1초당 주어지는 시간 -> 1000 / m_targetFPS
	DWORD m_standardServerTime;		// 서버 로직이 시작될 때 초기화되는 값
	DWORD m_currentServerTime;		// 서버 로직이 시작될 때 초기화되고, 이후에 프레임이 지날 때 마다 m_givenFrameTime 만큼 더함.

	UINT16 m_FPS;	// 현재 프레임
};


