#include <iostream>
#include <winsock2.h>
#include <thread>
#include <string>
#include <locale.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12346
#define BUFFER_SIZE 1024

SOCKET client_socket;
SOCKADDR_IN server_addr;
std::string client_name;

void receive_messages() {
    char buffer[BUFFER_SIZE];
    SOCKADDR_IN from_addr;
    int from_addr_size = sizeof(from_addr);

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = recvfrom(client_socket, buffer, BUFFER_SIZE, 0,
                                (SOCKADDR*)&from_addr, &from_addr_size);
        
        if (bytes_read > 0) {
            std::cout << std::string(buffer, bytes_read);
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
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 1;
    }

    // 3. Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    // 4. Get client name
    std::cout << "Enter your name: ";
    std::getline(std::cin, client_name);

    // 5. Send JOIN message
    std::string join_message = "JOIN:" + client_name;
    sendto(client_socket, join_message.c_str(), join_message.size(), 0,
          (SOCKADDR*)&server_addr, sizeof(server_addr));

    // 6. Start receive thread
    std::thread receive_thread(receive_messages);
    receive_thread.detach();

    std::cout << "Connected to server. Type your messages (type 'exit' to quit):" << std::endl;

    // 7. Main loop for sending messages
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        
        if (message == "exit") {
            // Send LEAVE message
            std::string leave_message = "LEAVE:" + client_name;
            sendto(client_socket, leave_message.c_str(), leave_message.size(), 0,
                  (SOCKADDR*)&server_addr, sizeof(server_addr));
            break;
        }

        // Send message with client name prefix
        std::string full_message = client_name + ":" + message;
        sendto(client_socket, full_message.c_str(), full_message.size(), 0,
              (SOCKADDR*)&server_addr, sizeof(server_addr));
    }

    closesocket(client_socket);
    WSACleanup();
    return 0;
} 