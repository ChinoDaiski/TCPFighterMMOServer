#pragma once

#include "Singleton.h"
#include <fstream>
#include <sstream>
#include <chrono>

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
    // 버퍼링된 로그 정보를 파일에 기록
    void SaveLogToFile(const std::string& fileName);

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

    // 바이너리 파일에 로그 저장
    template<typename... Args>
    void SaveLogToBinary(Args... args);

private:
    

private:
    //LogLevel currentLogLevel = dfLOG_LEVEL_DEBUG; // 기본 로그 레벨
    LogLevel currentLogLevel = dfLOG_LEVEL_ERROR; // 기본 로그 레벨

private:
    // 로그 버퍼링을 위한 변수
    std::ostringstream m_oss;

    // 기록 관련
    std::ofstream m_logFile;    // 지터와 FPS 데이터를 기록할 파일
    std::ofstream m_binLogFile; // 바이너리 로그 파일

    // 타임스탬프 함수 (std::chrono 사용)
    std::string GetCurrentTime(void);
};

template<typename ...Args>
inline void LogManager::LogMessage(LogLevel level, const std::string& levelStr, Args ...args)
{
    if (level >= currentLogLevel) {
        std::ostringstream oss;
        // 가변 인자들을 스트림에 추가
        (oss << ... << args); // Fold expression (C++17)

        std::ofstream logFile("log.txt", std::ios::app);
        if (logFile.is_open()) {
            logFile << "[" << GetCurrentTime() << "][" << levelStr << "] " << oss.str() << std::endl;
            logFile.close();
        }
    }
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

// 바이너리 파일에 로그 저장 함수
template<typename ...Args>
inline void LogManager::SaveLogToBinary(Args ...args)
{
    std::ostringstream oss;
    (oss << ... << args); // 가변 인자들을 스트림에 추가

    std::string logData = oss.str();

    std::ofstream binLogFile("log.bin", std::ios::binary | std::ios::app);
    if (binLogFile.is_open()) {
        size_t size = logData.size();
        binLogFile.write(reinterpret_cast<const char*>(&size), sizeof(size_t)); // 로그 크기 기록
        binLogFile.write(logData.c_str(), size); // 로그 데이터 기록
        binLogFile.close();
    }
}