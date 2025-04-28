#include <iostream>
#include <winsock2.h>
#include <thread>
#include <vector>
#include <mutex>
#include <map>
#include <ctime>
#include <locale.h>

#pragma comment(lib, "ws2_32.lib") // Link with Winsock library

#define PORT 12345

std::mutex clients_mutex;
std::map<SOCKET, std::string> clients;

void broadcast_message(const std::string& message, SOCKET sender_socket = INVALID_SOCKET) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (const auto& client : clients) {
        if (client.first != sender_socket) {
            send(client.first, message.c_str(), message.size(), 0);
        }
    }
}

void handle_client(SOCKET client_socket) {
    char buffer[1024] = { 0 };
    std::string client_name;

    // Receive client name
    int bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_read <= 0) {
        closesocket(client_socket);
        return;
    }
    client_name = std::string(buffer, bytes_read);

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients[client_socket] = client_name;
    }

    std::string welcome_message = client_name + " has joined the chat room.\n";
    std::cout << welcome_message;
    broadcast_message(welcome_message);

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_read <= 0) {
            std::string leave_message = client_name + " has left the chat room.\n";
            std::cout << leave_message;
            broadcast_message(leave_message);
            break;
        }

        // 현재 시간 구하기
        std::time_t now = std::time(nullptr);
        std::tm* local_tm = std::localtime(&now);
        char time_buf[16];
        std::strftime(time_buf, sizeof(time_buf), "[%H:%M:%S]", local_tm);

        // 메시지에 시간 추가
        std::string message = std::string(time_buf) + " " + client_name + ": " + std::string(buffer, bytes_read) + "\n";
        std::cout << message;
        broadcast_message(message, client_socket);
    }

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.erase(client_socket);
    }
    closesocket(client_socket);
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

    SOCKET server_socket;
    SOCKADDR_IN server_addr;

    // 2. Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
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

    // 5. Listen
    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed." << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "Chat server started (port " << PORT << ")..." << std::endl;

    std::vector<std::thread> threads;

    // 6. Wait for client connections
    while (true) {
        SOCKET client_socket;
        SOCKADDR_IN client_addr;
        int client_addr_size = sizeof(client_addr);

        client_socket = accept(server_socket, (SOCKADDR*)&client_addr, &client_addr_size);
        if (client_socket != INVALID_SOCKET) {
            std::cout << "New client connected!" << std::endl;
            threads.emplace_back(handle_client, client_socket);
        }
    }

    // 7. Wait for threads to finish
    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
