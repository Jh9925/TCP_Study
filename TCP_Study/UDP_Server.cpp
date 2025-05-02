#include <iostream>
#include <winsock2.h>
#include <thread>
#include <vector>
#include <mutex>
#include <map>
#include <ctime>
#include <locale.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 12346
#define BUFFER_SIZE 1024

SOCKET server_socket;  // 전역 변수로 선언
std::mutex clients_mutex;
std::map<std::string, SOCKADDR_IN> clients;  // 클라이언트 이름과 주소를 저장

void broadcast_message(const std::string& message, const std::string& sender_name = "") {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (const auto& client : clients) {
        if (client.first != sender_name) {
            sendto(server_socket, message.c_str(), message.size(), 0,
                  (SOCKADDR*)&client.second, sizeof(client.second));
        }
    }
}

int main() {
    setlocale(LC_ALL, "");

    // 1. Initialize Winsock
    WSADATA wsaData;
    int wsaInit = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaInit != 0) {
        std::cerr << "WSAStartup failed: " << wsaInit << std::endl;
        return 1;
    }

    SOCKADDR_IN server_addr;

    // 2. Create socket
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 1;
    }

    // 3. Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 4. Bind
    if (bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "UDP Chat server started (port " << PORT << ")..." << std::endl;

    char buffer[BUFFER_SIZE];
    SOCKADDR_IN client_addr;
    int client_addr_size = sizeof(client_addr);

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = recvfrom(server_socket, buffer, BUFFER_SIZE, 0,
                                (SOCKADDR*)&client_addr, &client_addr_size);

        if (bytes_read <= 0) continue;

        std::string message(buffer, bytes_read);
        
        // 메시지가 "JOIN:"으로 시작하면 새로운 클라이언트 등록
        if (message.substr(0, 5) == "JOIN:") {
            std::string client_name = message.substr(5);
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                clients[client_name] = client_addr;
            }
            std::string welcome_message = client_name + " has joined the chat room.\n";
            std::cout << welcome_message;
            broadcast_message(welcome_message);
        }
        // 메시지가 "LEAVE:"로 시작하면 클라이언트 제거
        else if (message.substr(0, 6) == "LEAVE:") {
            std::string client_name = message.substr(6);
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                clients.erase(client_name);
            }
            std::string leave_message = client_name + " has left the chat room.\n";
            std::cout << leave_message;
            broadcast_message(leave_message);
        }
        // 일반 메시지 처리
        else {
            // 메시지에서 클라이언트 이름 추출 (형식: "NAME:message")
            size_t pos = message.find(":");
            if (pos != std::string::npos) {
                std::string client_name = message.substr(0, pos);
                std::string actual_message = message.substr(pos + 1);

                // 현재 시간 구하기
                std::time_t now = std::time(nullptr);
                std::tm* local_tm = std::localtime(&now);
                char time_buf[16];
                std::strftime(time_buf, sizeof(time_buf), "[%H:%M:%S]", local_tm);

                std::string formatted_message = std::string(time_buf) + " " + client_name + ": " + actual_message + "\n";
                std::cout << formatted_message;
                broadcast_message(formatted_message, client_name);
            }
        }
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
} 