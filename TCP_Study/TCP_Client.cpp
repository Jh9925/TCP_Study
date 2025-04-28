#include <iostream>
#include <winsock2.h>
#include <string>
#include <ws2tcpip.h>
#include <thread>
#include <locale.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345

void receive_messages(SOCKET sock) {
    char buffer[1024] = { 0 };
    while (true) {
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            std::cout << "Disconnected from server." << std::endl;
            break;
        }
        buffer[bytes_received] = '\0';
        std::cout << buffer;
        if (buffer[bytes_received - 1] != '\n') {
            std::cout << std::endl;
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

    // 2. Create socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 1;
    }

    // 3. Set server address
    SOCKADDR_IN server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Convert "127.0.0.1" to binary form
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        std::cerr << "Address conversion failed." << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // 4. Connect to server
    if (connect(sock, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server." << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server!" << std::endl;

    // Enter user name
    std::string name;
    std::cout << "Enter your name: ";
    std::getline(std::cin, name);
    send(sock, name.c_str(), name.size(), 0);

    // Start message receiving thread
    std::thread receiver(receive_messages, sock);
    receiver.detach();

    // Send messages
    while (true) {
        std::string message;
        std::getline(std::cin, message);

        if (message == "/quit") {
            break;
        }

        send(sock, message.c_str(), message.size(), 0);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}