#include <iostream>
#include <string>
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

constexpr uint16_t TCP_PORT = 9001;
constexpr size_t MAX_MESSAGE_BUFFER_SIZE = 1024;

// Wrapper to unify socket differences between Windows and POSIX
struct cross_platform_socket {
#ifdef _WIN32
    using socket_type = SOCKET;

    static void initialize_winsock() { 
        WSADATA wsa_data; 
        WSAStartup(MAKEWORD(2,2), &wsa_data); 
    }

    static void cleanup_winsock() { WSACleanup(); }
    static void close_socket(SOCKET socket_descriptor) { closesocket(socket_descriptor); }
#else
    using socket_type = int;

    static void initialize_winsock() {}
    static void cleanup_winsock() {}
    static void close_socket(int socket_descriptor) { close(socket_descriptor); }
#endif
};

// Server: listens for a single TCP connection
void run_tcp_server() {
    cross_platform_socket::socket_type server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) { perror("Failed to create server socket"); return; }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(TCP_PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int reuse_address_option = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_address_option, sizeof(reuse_address_option));

    if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) < 0) { perror("Bind failed"); return; }
    if (listen(server_socket, 1) < 0) { perror("Listen failed"); return; }

    std::cout << "Server listening on TCP port " << TCP_PORT << "...\n";

    sockaddr_in client_address{};
#ifdef _WIN32
    int client_address_length = sizeof(client_address);
#else
    socklen_t client_address_length = sizeof(client_address);
#endif

    cross_platform_socket::socket_type client_socket = accept(server_socket, (sockaddr*)&client_address, &client_address_length);
    if (client_socket < 0) { perror("Accept failed"); return; }

    char received_message_buffer[MAX_MESSAGE_BUFFER_SIZE]{};
    int received_byte_count = recv(client_socket, received_message_buffer, MAX_MESSAGE_BUFFER_SIZE - 1, 0);
    if (received_byte_count > 0) {
        std::cout << "Server received message: " 
                  << std::string(received_message_buffer, received_byte_count) 
                  << "\n";
    }

    cross_platform_socket::close_socket(client_socket);
    cross_platform_socket::close_socket(server_socket);
}

// Client: connects to server_ip and sends a TCP message
void run_tcp_client(const std::string& server_ip) {
    cross_platform_socket::socket_type client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) { perror("Failed to create client socket"); return; }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(TCP_PORT);

#ifdef _WIN32
    InetPton(AF_INET, server_ip.c_str(), &server_address.sin_addr);
#else
    inet_pton(AF_INET, server_ip.c_str(), &server_address.sin_addr);
#endif

    if (connect(client_socket, (sockaddr*)&server_address, sizeof(server_address)) < 0) { perror("Connection failed"); return; }

    std::string client_message = "Hello from C++ client\n";
    send(client_socket, client_message.c_str(), client_message.size(), 0);

    cross_platform_socket::close_socket(client_socket);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " server|client [SERVER_IP]\n";
        return 1;
    }

    cross_platform_socket::initialize_winsock();

    std::string run_mode = argv[1];
    if (run_mode == "server") {
        run_tcp_server();
    } else if (run_mode == "client") {
        if (argc < 3) { 
            std::cerr << "Client mode requires SERVER_IP argument\n"; 
            return 1; 
        }
        std::string server_ip_address = argv[2];
        run_tcp_client(server_ip_address);
    } else {
        std::cerr << "Unknown mode: " << run_mode << "\n";
    }

    cross_platform_socket::cleanup_winsock();
    return 0;
}
