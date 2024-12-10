#pragma once

#include "Singleton.h"
#include <fstream>
#include <sstream>
#include <chrono>

#include "CircularQueue.h"

// 로그 레벨 정의
enum LogLevel {
    dfLOG_LEVEL_DEBUG = 0,
    dfLOG_LEVEL_ERROR,
    dfLOG_LEVEL_SYSTEM
};

// 로그 매니저 클래스
class LogManager : public SingletonBase<LogManager> {
private:
    friend class SingletonBase<LogManager>;

public:
    explicit LogManager() noexcept;
    ~LogManager() noexcept;

    // 복사 생성자와 대입 연산자를 삭제하여 복사 방지
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;

public:
    // 현재 로그 레벨 설정
    void SetLogLevel(LogLevel level);

    // 로그 메시지 작성 함수
    template<typename... Args>
    void LogMessage(LogLevel level, const std::string& levelStr, Args... args);

    // 로그 출력 함수
    template<typename... Args>
    void LogDebug(Args... args);

    template<typename... Args>
    void LogError(Args... args);

    template<typename... Args>
    void LogSystem(Args... args);

public:
    void saveLog(void);


private:
    LogLevel currentLogLevel = dfLOG_LEVEL_DEBUG; // 기본 로그 레벨
    //LogLevel currentLogLevel = dfLOG_LEVEL_ERROR; // 기본 로그 레벨

private:
    // 기록 관련
    std::ofstream m_logFile;    // 지터와 FPS 데이터를 기록할 파일

    // 타임스탬프 함수 (std::chrono 사용)
    std::string GetCurrentTime(void);

    CircularQueue<std::string> logQueue;
};

template<typename ...Args>
inline void LogManager::LogMessage(LogLevel level, const std::string& levelStr, Args ...args)
{
    //if (level >= currentLogLevel) {
    //    std::ostringstream oss;
    //    // 가변 인자들을 스트림에 추가
    //    (oss << ... << args); // Fold expression (C++17)

    //    logQueue.enqueue(oss.str());

    //    //if (m_logFile.is_open()) {
    //    //    m_logFile << "[" << GetCurrentTime() << "][" << levelStr << "] " << oss.str() << std::endl;
    //    //    m_logFile.close();
    //    //}
    //}
}

template<typename ...Args>
inline void LogManager::LogDebug(Args ...args)
{
    do {
        LogMessage(dfLOG_LEVEL_DEBUG, "dfLOG_LEVEL_DEBUG", args...);
    } while (0);
}

template<typename ...Args>
inline void LogManager::LogError(Args ...args)
{
    do {
        LogMessage(dfLOG_LEVEL_ERROR, "dfLOG_LEVEL_ERROR", args...);
    } while (0);
}

template<typename ...Args>
inline void LogManager::LogSystem(Args ...args)
{
    do {
        LogMessage(dfLOG_LEVEL_SYSTEM, "dfLOG_LEVEL_SYSTEM", args...);
    } while (0);
}