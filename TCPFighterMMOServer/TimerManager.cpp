
#include "pch.h"
#include "TimerManager.h"

#include <ctime>    // 시간 관련 라이브러리
#include <sstream>  // 스트링 스트림 사용
#include <iomanip>  // 시간 포맷을 위한 헤더

#include "DebugDefines.h"
#include "LogManager.h"

static LogManager& logManager = LogManager::GetInstance();

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

	// 지터용, 여기서 시간 측정을 했기에 MeasureJitter에서 매번 m_prevNetIOTime이 0인지 검사를 안하고 바로 사용 가능. 처음에 튀는건 감수
	m_prevNetIOTime = m_standardServerTime;
}

bool CTimerManager::CheckFrame(void)
{
	// 현재 로직 시간 업데이트
	m_currentMainLogicTime = timeGetTime();

#ifdef MEASURE_GITTER

	// 지터 측정
	MeasureJitter();

#endif // MEASURE_GITTER

#ifdef CHECK_FPS

	// 프레임 맞추기
	if (m_currentMainLogicTime < (m_currentServerTime + m_givenFrameTime))
		return false;

	m_currentServerTime += m_givenFrameTime;

	m_FPS++;

	// fps 측정
	if ((m_currentServerTime - m_standardServerTime) >= 1000)
	{
		std::cout << "Logic Frame"<< m_FPS << "\n"; //" / " << m_currentServerTime - m_standardServerTime << "\n";
		m_standardServerTime += 1000;

		m_FPS = 0;

#ifdef CHECK_GITTER

		// 지터 출력
		PrintJitterStats();

#endif // CHECK_GITTER
	}

	return true;

#else

	// 프레임 맞추기
	if (m_currentMainLogicTime < (m_currentServerTime + m_givenFrameTime))
		return false;
	

	m_currentServerTime += m_givenFrameTime;

	//// 프레임이 밀렸을 경우 서버 시간을 당겨옴.
	//while (m_currentMainLogicTime > m_currentServerTime)
	//{
	//	m_currentServerTime += m_givenFrameTime;
	//}

#ifdef CHECK_GITTE

	// 1초마다 지터 통계 출력
	if ((m_currentServerTime - m_standardServerTime) >= 1000) {
		m_standardServerTime += 1000;
		PrintJitterStats();
	}

#endif // CHECK_GITTER

	return true;
	
#endif
}

void CTimerManager::MeasureJitter(void)
{
	m_currNetIOTime = timeGetTime();	// m_currentMainLogicTime를 사용하지말고, 정확한 측정을 위해 따로 한번 더 측정(timeGetTime).

	// 이전 네트워크 프레임과 현재 네트워크 프레임간의 차이를 계산 
	DWORD delay = m_currNetIOTime - m_prevNetIOTime;

	// m_givenFrameTime과의 차이를 계산
	m_jitter = abs(static_cast<int>(delay) - static_cast<int>(m_givenFrameTime));

	// 1초 동안의 지터 통계 계산
	if (m_jitterCount == 0) {
		m_minJitter = m_jitter;
		m_maxJitter = m_jitter;
	}
	else {
		if (m_jitter < m_minJitter) m_minJitter = m_jitter;
		if (m_jitter > m_maxJitter) m_maxJitter = m_jitter;
	}

	m_totalJitter += m_jitter;
	m_jitterCount++;

	m_prevNetIOTime = m_currNetIOTime;
}

void CTimerManager::PrintJitterStats(void)
{
	if (m_jitterCount > 0) {
		DWORD avgJitter = m_totalJitter / m_jitterCount;
		//std::cout << "Jitter - Min: " << m_minJitter << " ms, Max: " << m_maxJitter << " ms, Avg: " << avgJitter << " ms\n"; 
		
		// 파일에 기록
		logManager.LogDebug("Jitter\nMin: ", m_minJitter, " ms\nMax: ", m_maxJitter, " ms\nAvg: ", avgJitter, " ms\n\n");
	}

	// 통계 초기화
	m_minJitter = 0;
	m_maxJitter = 0;
	m_totalJitter = 0;
	m_jitterCount = 0;
}
