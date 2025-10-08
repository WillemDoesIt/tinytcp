#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

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

const std::string VERSION = "tcp 0.2.1";
const size_t MAX_MESSAGE_BUFFER_SIZE = 4096;

struct cross_platform_socket {
#ifdef _WIN32
    using socket_type = SOCKET;

    static void initialize() { 
        WSADATA wsa_data; 
        WSAStartup(MAKEWORD(2,2), &wsa_data); 
    }
    static void cleanup() { WSACleanup(); }
    static void close_socket(SOCKET s) { closesocket(s); }
#else
    using socket_type = int;

    static void initialize() {}
    static void cleanup() {}
    static void close_socket(int s) { close(s); }
#endif
};

std::string get_config_path() {
    const char* home = getenv("HOME");
#ifdef _WIN32
    if (!home) home = getenv("USERPROFILE");
#endif
    std::string path = std::string(home) + "/.config/tcp/config.toml";
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    return path;
}

int get_port_from_config() {
    std::string path = get_config_path();
    int port = 9001;
    std::ifstream in(path);
    if (in) {
        std::string line;
        while (std::getline(in, line)) {
            if (line.rfind("port", 0) == 0) {
                std::istringstream iss(line.substr(line.find('=') + 1));
                iss >> port;
            }
        }
    } else {
        std::ofstream out(path);
        out << "# tcp config\nport = " << port << "\n";
    }
    return port;
}

void print_help() {
    std::cout <<
    "Usage:\n"
    "  tcp server [--port <num>]\n"
    "  tcp client <ip> [message|file] [--port <num>]\n\n"
    "Options:\n"
    "  --help        Show this help message\n"
    "  --version     Show version info\n"
    "  --port <num>  Override port (default from config)\n"
    "\nConfig file: ~/.config/tcp/config.toml\n";
}

void run_tcp_server(int port) {
    cross_platform_socket::socket_type server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) { perror("socket() failed"); return; }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int reuse_addr = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_addr, sizeof(reuse_addr));

    if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) < 0) { perror("bind() failed"); return; }
    if (listen(server_socket, 1) < 0) { perror("listen() failed"); return; }

    std::cout << "Server listening on TCP port " << port << "...\n";

    sockaddr_in client_address{};
#ifdef _WIN32
    int client_addr_len = sizeof(client_address);
#else
    socklen_t client_addr_len = sizeof(client_address);
#endif
    cross_platform_socket::socket_type client_socket = accept(server_socket, (sockaddr*)&client_address, &client_addr_len);
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

void run_tcp_client(const std::string& server_ip, int port, const std::string& msg) {
    cross_platform_socket::socket_type client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) { perror("socket() failed"); return; }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

#ifdef _WIN32
    InetPton(AF_INET, server_ip.c_str(), &server_address.sin_addr);
#else
    inet_pton(AF_INET, server_ip.c_str(), &server_address.sin_addr);
#endif

    if (connect(client_socket, (sockaddr*)&server_address, sizeof(server_address)) < 0) { perror("connect() failed"); return; }

    send(client_socket, msg.c_str(), msg.size(), 0);
    cross_platform_socket::close_socket(client_socket);
}

int main(int argc, char** argv) {
    if (argc > 1) {
        std::string arg1 = argv[1];
        if (arg1 == "--help" || arg1 == "-h") { print_help(); return 0; }
        if (arg1 == "--version" || arg1 == "-v") { std::cout << VERSION << "\n"; return 0; }
    }

    bool is_server = argc > 1 && std::string(argv[1]) == "server";
    int port = get_port_from_config();
    for (int i = 1; i < argc; ++i)
        if (std::string(argv[i]) == "--port" && i + 1 < argc)
            port = std::stoi(argv[i + 1]);

    cross_platform_socket::initialize();

    if (is_server) {
        run_tcp_server(port);
    } else {
        if (argc < 3) { std::cerr << "Client requires SERVER_IP argument\n"; return 1; }
        std::string server_ip = argv[2];

        std::string msg = "Hello from C++ client\n";
        if (argc >= 4) {
            std::ifstream f(argv[3]);
            if (f.good()) {
                std::ostringstream ss;
                ss << f.rdbuf();
                msg = ss.str();
            } else {
                msg = argv[3];
            }
        }

        run_tcp_client(server_ip, port, msg);
    }

    cross_platform_socket::cleanup();
    return 0;
}
