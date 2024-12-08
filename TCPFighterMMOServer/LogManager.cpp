
#include "pch.h"
#include "LogManager.h"

LogManager::LogManager() noexcept
{
	//===================================================
	// 현재 시간을 기반으로 로그를 기록하는 파일 생성
	//===================================================

	// 현재 시간을 가져옴
	time_t now = time(nullptr);
	struct tm timeInfo;
	localtime_s(&timeInfo, &now);  // 현재 시간을 구조체에 저장

	// 시간 정보를 기반으로 파일명을 생성
	std::ostringstream oss;
	oss << "server_performance_log_"
		<< std::put_time(&timeInfo, "%Y-%m-%d_%H-%M-%S")  // 날짜와 시간을 "YYYY-MM-DD_HH-MM-SS" 형식으로
		<< ".txt";

	std::string fileName = oss.str();

	// 로그 파일 열기
	m_logFile.open(fileName, std::ios::out | std::ios::app);
	if (!m_logFile.is_open()) {
		std::cerr << "Failed to open log file: " << fileName << std::endl;
	}
}

LogManager::~LogManager() noexcept
{
    // 로그 파일 닫기
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
}

void LogManager::SetLogLevel(LogLevel level)
{
    currentLogLevel = level;
}

void LogManager::saveLog(void)
{
	// 로그 파일 닫기
	if (m_logFile.is_open()) {
		for (int i = 0; i < logQueue.rear; ++i)
			m_logFile << logQueue.queue[i] << std::endl;

		logQueue.rear = 0;
	}
}

std::string LogManager::GetCurrentTime(void)
{
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now;

    // 쓰레드 안전한 방법으로 로컬 시간 변환 (POSIX: localtime_r, Windows: localtime_s)
#ifdef _WIN32
    localtime_s(&tm_now, &time_t_now);
#else
    localtime_r(&time_t_now, &tm_now);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm_now, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
