#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #pragma comment(lib, "ws2_32.lib")
  using socklen_t = int;
  #define CLOSESOCKET closesocket
  void init_sockets() { WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa); }
  void cleanup_sockets() { WSACleanup(); }
  int inet_pton_win(int af, const char* src, void* dst) { return InetPtonA(af, src, dst); }
  #define inet_pton inet_pton_win
#else
  #include <arpa/inet.h>
  #include <unistd.h>
  #define SOCKET int
  #define CLOSESOCKET close
  void init_sockets() {}
  void cleanup_sockets() {}
#endif

#include "toml.hpp"
#include <iostream>
#include <cstring>
#include <filesystem>
#include <fstream>

std::filesystem::path get_config_path() {
#ifdef _WIN32
    const char* appdata = std::getenv("APPDATA");
    std::filesystem::path base = appdata ? appdata : ".";
    base /= "tinytcp";
#else
    const char* home = std::getenv("HOME");
    std::filesystem::path base = home ? home : ".";
    base /= ".config/tinytcp";
#endif
    std::filesystem::create_directories(base);
    return base / "config.toml";
}

struct Config {
    int port = 49153;
    std::string message = "Hello from client!";
};

Config load_config() {
    Config cfg;
    auto path = get_config_path();

    if (!std::filesystem::exists(path)) {
        std::ofstream out(path);
        out << "port = 49153\nmessage = \"Hello from client!\"\n";
        std::cout << "Created default config at " << path << "\n";
        return cfg;
    }

    try {
        auto tbl = toml::parse_file(path.string());
        if (auto p = tbl["port"].value<int>()) cfg.port = *p;
        if (auto m = tbl["message"].value<std::string>()) cfg.message = *m;
    } catch (const toml::parse_error& err) {
        std::cerr << "Error parsing " << path << ": " << err.description() << "\n";
    }

    return cfg;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [server|client] [host]\n";
        return 1;
    }

    init_sockets();
    Config cfg = load_config();

    if (std::strcmp(argv[1], "server") == 0) {
        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(cfg.port);
        bind(sock, (sockaddr*)&addr, sizeof(addr));
        listen(sock, 1);
        std::cout << "Server listening on port " << cfg.port << "...\n";

        SOCKET client = accept(sock, nullptr, nullptr);
        char buf[1024]{};
        int bytes = recv(client, buf, sizeof(buf)-1, 0);
        if (bytes > 0) std::cout << "Received: " << buf << "\n";
        CLOSESOCKET(client);
        CLOSESOCKET(sock);
    } 
    else if (std::strcmp(argv[1], "client") == 0) {
        const char* host = (argc > 2) ? argv[2] : "127.0.0.1";
        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(cfg.port);
        inet_pton(AF_INET, host, &addr.sin_addr);

        if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == 0) {
            send(sock, cfg.message.c_str(), cfg.message.size(), 0);
            std::cout << "Message sent to " << host << ":" << cfg.port << "\n";
        } else {
            std::cerr << "Failed to connect to " << host << "\n";
        }
        CLOSESOCKET(sock);
    }

    cleanup_sockets();
}

