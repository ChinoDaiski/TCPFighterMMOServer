
#include "pch.h"
#include "TimerManager.h"

//#define CHECK_FPS

CTimerManager::CTimerManager() noexcept
{
}

CTimerManager::~CTimerManager() noexcept
{
}

void CTimerManager::InitTimer(DWORD _targetFPS)
{
	timeBeginPeriod(1);

	m_targetFPS = _targetFPS;
	m_givenFrameTime = 1000 / m_targetFPS;
	m_standardServerTime = timeGetTime();
	m_currentServerTime = m_standardServerTime;
}

bool CTimerManager::CheckFrame(void)
{
	// 프레임 맞추기
	if (timeGetTime() < (m_currentServerTime + m_givenFrameTime))
		return false;

	m_currentServerTime += m_givenFrameTime;

#ifdef CHECK_FPS
	m_FPS++;

	// fps 측정
	if ((m_currentServerTime - m_standardServerTime) >= 1000)
	{
		std::cout << m_FPS << " / " << m_currentServerTime - m_standardServerTime << "\n";

		m_FPS = 0;
	}
#endif // CHECK_FPS

	return true;
}

