
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
#ifdef CHECK_FPS

	// 프레임 맞추기
	if (timeGetTime() < (m_currentServerTime + m_givenFrameTime))
		return false;

	m_currentServerTime += m_givenFrameTime;

	m_FPS++;

	// fps 측정
	if ((m_currentServerTime - m_standardServerTime) >= 1000)
	{
		std::cout << m_FPS << "\n"; //" / " << m_currentServerTime - m_standardServerTime << "\n";
		m_standardServerTime += 1000;

		m_FPS = 0;
	}

	return true;

#else

	// 프레임 맞추기
	if (timeGetTime() < (m_currentServerTime + m_givenFrameTime))
		return false;

	m_currentServerTime += m_givenFrameTime;

	return true;
	
#endif
}