#include <iostream>
#include <fstream>
#include <string>

void SearchLogAndSave(const std::string& searchKey1, const std::string& searchKey2) {
    std::ifstream logFile("log.bin", std::ios::binary);
    if (logFile.is_open()) {
        std::ofstream resultFile("filtered_logs.txt");
        while (true) {
            // 로그 크기 읽기
            size_t size;
            logFile.read(reinterpret_cast<char*>(&size), sizeof(size_t));
            if (logFile.eof()) break; // 파일 끝에 도달 시 종료

            // 메시지 읽기
            std::string message(size, '\0');
            logFile.read(&message[0], size);

            // 조건에 맞는 메시지 찾기 (MOVE_START 패킷과 MY ID : 13 검색)
            if (message.find(searchKey1) != std::string::npos && message.find(searchKey2) != std::string::npos) {
                // 검색된 로그를 텍스트 파일에 저장
                resultFile << message << std::endl;
            }
        }
        logFile.close();
        resultFile.close();
    }
    else {
        std::cerr << "Error: Unable to open log.bin" << std::endl;
    }
}

// 검색 호출 예시
int main() {
    SearchLogAndSave("MOVE_START", "MY ID : 13"); // MY ID : 13으로 수정
    return 0;
}
