#include "network.hpp"
#include <iostream>
#include <cstring>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#endif

const size_t MAX_MESSAGE_BUFFER_SIZE = 4096;

#ifdef _WIN32
void cross_platform_socket::initialize() {
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2,2), &wsa_data);
}
void cross_platform_socket::cleanup() { WSACleanup(); }
void cross_platform_socket::close_socket(SOCKET s) { closesocket(s); }
#else
void cross_platform_socket::close_socket(int s) { close(s); }
#endif

void start_server(int port) {
    cross_platform_socket::socket_type server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) { perror("socket() failed"); return; }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int reuse = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    if (bind(server_socket, (sockaddr*)&addr, sizeof(addr)) < 0) {
        if (errno == EACCES)
            std::cerr << "[!] Port " << port << " blocked by firewall or requires elevated privileges.\n";
        else if (errno == EADDRINUSE)
            std::cerr << "[!] Port " << port << " already in use.\n";
        else
            perror("bind() failed");
        return;
    }
    if (listen(server_socket, 10) < 0) { perror("listen() failed"); return; }

    std::cout << "Server listening on TCP port " << port << "...\n";

    sockaddr_in client_addr{};
#ifdef _WIN32
    int client_len = sizeof(client_addr);
#else
    socklen_t client_len = sizeof(client_addr);
#endif
    cross_platform_socket::socket_type client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_len);
    if (client_socket < 0) { perror("accept() failed"); return; }

    char buffer[MAX_MESSAGE_BUFFER_SIZE]{};
    int n;
    while ((n = recv(client_socket, buffer, MAX_MESSAGE_BUFFER_SIZE - 1, 0)) > 0) {
        buffer[n] = '\0';
        std::cout << buffer;
    }
    std::cout << "\nConnection closed.\n";

    cross_platform_socket::close_socket(client_socket);
    cross_platform_socket::close_socket(server_socket);
}

void send_message(const std::string& server_ip, int port, const std::string& msg) {
    cross_platform_socket::socket_type sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket() failed"); return; }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

#ifdef _WIN32
    InetPton(AF_INET, server_ip.c_str(), &addr.sin_addr);
#else
    inet_pton(AF_INET, server_ip.c_str(), &addr.sin_addr);
#endif

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        if (errno == EACCES)
            std::cerr << "[!] Cannot connect to " << server_ip << ":" << port << " (blocked by firewall?)\n";
        else if (errno == ECONNREFUSED)
            std::cerr << "[!] Connection refused by " << server_ip << ":" << port << "\n";
        else
            perror("connect() failed");
        cross_platform_socket::close_socket(sock);
        return;
    }

    send(sock, msg.c_str(), msg.size(), 0);
    shutdown(sock, SHUT_WR);  // <- add this line
    cross_platform_socket::close_socket(sock);

}
