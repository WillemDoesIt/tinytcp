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
  #include <netdb.h>
  #define SOCKET int
  #define CLOSESOCKET close
  void init_sockets() {}
  void cleanup_sockets() {}
#endif

#include "toml.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <vector>
#include <cstring>

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
        out << "port = " << cfg.port << "\nmessage = \"" << cfg.message <<"\"\n";
        std::cout << "Created default config at " << path << "\n";
        return cfg;
    }
    try {
        auto tbl = toml::parse_file(path.string());
        if (auto p = tbl["port"].value<int>()) { 
            cfg.port = *p;
        }
        if (auto m = tbl["message"].value<std::string>()) {
            cfg.message = *m;
        }
    } catch (const toml::parse_error& err) {
        std::cerr << "Error parsing " << path << ": " << err.description() << "\n";
    }
    return cfg;
}

struct Args {
    std::string mode;
    std::string host = "127.0.0.1";
    int port = -1;
    std::string message;
    bool help = false;
    bool help_verbose = false;
    bool version = false;
};

void print_help(const Args& a) {
    if (a.help_verbose) {
        std::cout << R"(
tinytcp — a minimal cross-platform TCP testing tool

Usage:
  tinytcp [mode] [args]

Modes:
  client   Connect to a server and send a message.
  server   Listen for a connection and print received data.

Config:
  tinytcp automatically reads a config file at:
    )" << get_config_path() << R"(
  If it does not exist, it will be created automatically.
  Example contents:
    port = 49153
    message = "Hello from client!"

  The config sets default values for port and message.
  Command-line flags override these at runtime.

Client usage:
  tinytcp client <host> [options]
  Options:
    -m, --message <text>   Message to send (overrides config)
    -p, --port <port>      Port to connect to
    -h, --help             Show help for client

Server usage:
  tinytcp server [options]
  Options:
    -p, --port <port>      Port to listen on
    -h, --help             Show help for server

Examples:
  tinytcp server
  tinytcp client 127.0.0.1
  tinytcp client 127.0.0.1 --port 50001 --message "test"

Notes:
  • tinytcp reads its config once at startup — changes mid-run have no effect.
  • communication will stall if the server device has a firewall blocking the TCP port, which is common on many operating systems. You may have to manually enable ports or disable the firewall, be careful when doing so.

Version:
  tinytcp v0.0.1-beta
        )";
        return;
    }

    if (a.mode.empty()) {
        std::cout <<
            "tinytcp [mode] [args]\n"
            "  Modes:\n"
            "    client   Connect to a server and send message\n"
            "    server   Listen for incoming connection\n"
            "  Global Options:\n"
            "    -h, --help            Show help\n"
            "    --help-verbose        Detailed manual\n"
            "    -v, --version         Show version\n"
            "  Config file: " << get_config_path() << "\n\n";
        return;
    }

    if (a.mode == "server") {
        std::cout <<
            "tinytcp server [options]\n"
            "  -p, --port <port>     Port to listen on (default from config)\n"
            "  -h, --help            Show this help\n\n";
        return;
    }

    if (a.mode == "client") {
        std::cout <<
            "tinytcp client [host] [options]\n"
            "  -m, --message <msg>   Message to send (default from config)\n"
            "  -p, --port <port>     Port (default from config)\n"
            "  -h, --help            Show this help\n\n";
        return;
    }
}

Args parse_args(int argc, char** argv) {
    Args args;
    if (argc < 2) { args.help = true; return args; }

    args.mode = argv[1];
    std::vector<std::string> tokens(argv + 2, argv + argc);

    for (size_t i = 0; i < tokens.size(); ++i) {
        const std::string& t = tokens[i];
        if (t == "-h" || t == "--help") args.help = true;
        else if (t == "--help-verbose") args.help_verbose = true;
        else if (t == "-v" || t == "--version") args.version = true;
        else if ((t == "-p" || t == "--port") && i + 1 < tokens.size())
            args.port = std::stoi(tokens[++i]);
        else if ((t == "-m" || t == "--message") && i + 1 < tokens.size())
            args.message = tokens[++i];
        else if (args.mode == "client" && args.host == "127.0.0.1" && t.find('-') != 0)
            args.host = t;
        else
            throw std::runtime_error("Unknown or misplaced argument: " + t);
    }
    return args;
}

std::string get_local_ipv4() {
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return "unknown";

    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(53); // DNS port, doesn't actually send
    inet_pton(AF_INET, "8.8.8.8", &serv.sin_addr);

    if (connect(sock, (sockaddr*)&serv, sizeof(serv)) < 0) {
        CLOSESOCKET(sock);
        return "unknown";
    }

    sockaddr_in name{};
    socklen_t namelen = sizeof(name);
    if (getsockname(sock, (sockaddr*)&name, &namelen) < 0) {
        CLOSESOCKET(sock);
        return "unknown";
    }

    char buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &name.sin_addr, buf, sizeof(buf));
    CLOSESOCKET(sock);
    return std::string(buf);
}

void run_server(int port) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    bind(sock, (sockaddr*)&addr, sizeof(addr));
    listen(sock, 1);
    std::cout << "Server listening " << get_local_ipv4() << ":" << port << "...\n";

    SOCKET client = accept(sock, nullptr, nullptr);
    char buf[1024]{};
    int bytes = recv(client, buf, sizeof(buf)-1, 0);
    if (bytes > 0) std::cout << "Received: " << buf << "\n";
    CLOSESOCKET(client);
    CLOSESOCKET(sock);
}


void run_client(const std::string& host, int port, const std::string& msg) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == 0) {
        send(sock, msg.c_str(), msg.size(), 0);
        std::cout << "Message sent to " << host << ":" << port << "\n";
    } else {
        std::cerr << "Failed to connect to " << host << "\n";
    }
    CLOSESOCKET(sock);
}

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout << "Usage: tinytcp [mode] [options]\n"
                     "For more details: tinytcp --help\n";
        return 0;
    }

    std::string first = argv[1];
    if (first == "-v" || first == "--version") {
        std::cout << "tinytcp v0.0.1-beta\n";
        return 0;
    }
    if (first == "-h" || first == "--help" || first == "--help-verbose") {
        Args a;
        a.help = (first != "--help-verbose");
        a.help_verbose = (first == "--help-verbose");
        print_help(a);
        return 0;
    }

    Args args;
    try { args = parse_args(argc, argv); }
    catch (const std::exception& e) { std::cerr << e.what() << "\n"; return 1; }

    // handle help/version *inside* mode before networking
    if (args.help || args.help_verbose) {
        print_help(args);
        return 0;
    }
    if (args.version) {
        std::cout << "tinytcp v0.0.1-beta\n";
        return 0;
    }

    Config cfg = load_config();
    int port = (args.port != -1) ? args.port : cfg.port;
    std::string msg = !args.message.empty() ? args.message : cfg.message;

    init_sockets();

    if (args.mode == "server") run_server(port);
    else if (args.mode == "client") run_client(args.host, port, msg);
    else { std::cerr << "Unknown mode: " << args.mode << "\n"; return 1; }

    cleanup_sockets();
}

