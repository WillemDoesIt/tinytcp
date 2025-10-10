#pragma once
#include <string>

struct cross_platform_socket {
#ifdef _WIN32
    using socket_type = SOCKET;
    static void initialize();
    static void cleanup();
    static void close_socket(SOCKET s);
#else
    using socket_type = int;
    static void initialize() {}
    static void cleanup() {}
    static void close_socket(int s);
#endif
};

void start_server(int port);
void send_message(const std::string& server_ip, int port, const std::string& msg);
