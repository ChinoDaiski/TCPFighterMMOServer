#include <iostream>
#include <fstream>
#include <string>

void SearchLogAndSave(const std::string& searchKey1, const std::string& searchKey2) {
    std::ifstream logFile("log.bin", std::ios::binary);
    if (logFile.is_open()) {
        std::ofstream resultFile("filtered_logs.txt");
        while (true) {
            // �α� ũ�� �б�
            size_t size;
            logFile.read(reinterpret_cast<char*>(&size), sizeof(size_t));
            if (logFile.eof()) break; // ���� ���� ���� �� ����

            // �޽��� �б�
            std::string message(size, '\0');
            logFile.read(&message[0], size);

            // ���ǿ� �´� �޽��� ã�� (MOVE_START ��Ŷ�� MY ID : 13 �˻�)
            if (message.find(searchKey1) != std::string::npos && message.find(searchKey2) != std::string::npos) {
                // �˻��� �α׸� �ؽ�Ʈ ���Ͽ� ����
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

// �˻� ȣ�� ����
int main() {
    SearchLogAndSave("MOVE_START", "MY ID : 13"); // MY ID : 13���� ����
    return 0;
}
