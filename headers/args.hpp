#include "config.hpp"

#include <vector>

const std::string VERSION = "v0.4.0-beta";

struct Args {
    std::string mode;
    std::string host = "127.0.0.1";
    int port = -1;
    std::string message;
    bool help = false;
    bool help_verbose = false;
    bool version = false;
};

Args parse_args(int argc, char** argv) {
    Args args;

    if (argc < 2) { args.help = true; return args; }

    const std::string FIRST = argv[1];

    if (FIRST == "--help-verbose") { args.help_verbose = true; return args; }
    if (FIRST == "-v" || FIRST == "--version") { args.version = true; return args; }
    if (FIRST == "-h" || FIRST == "--help") { args.help = true; return args; }

    args.mode = FIRST;
    const std::vector<std::string> TOKENS(argv + 2, argv + argc);

    for (size_t i = 0; i < TOKENS.size(); ++i) {
        auto& t = TOKENS[i];
        if (t == "-h" || t == "--help") args.help = true;
        else if (t == "--help-verbose") args.help_verbose = true;
        else if (t == "-v" || t == "--version") args.version = true;
        else if ((t == "-p" || t == "--port") && i + 1 < TOKENS.size())
            args.port = std::stoi(TOKENS[++i]);
        else if ((t == "-m" || t == "--message") && i + 1 < TOKENS.size())
            args.message = TOKENS[++i];
        else if (args.mode == "client" && args.host == "127.0.0.1" && t[0] != '-')
            args.host = t;
        else
            throw std::runtime_error("Unknown or misplaced argument: " + t);
    }
    return args;
}

void print_help(const Args& A) {
    if (A.help_verbose) {
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
  • tinytcp reads its config once at startup. No changes mid-run have any effect.
  • communication will stall if the server device has a firewall blocking the TCP port, which is common on many operating systems. You may have to manually enable ports or disable the firewall, be careful when doing so.

Version:
  tinytcp v0.4.0-beta
        )";
        return;
    }

    if (A.mode.empty()) {
        std::cout <<
            "tinytcp [mode] [args]\n"
            "  Modes:\n"
            "    client   Connect to a server and send message\n"
            "    server   Listen for incoming connection\n"
            "  Global Options:\n"
            "    -h, --help            Show help\n"
            "    --help-verbose        Detailed manual\n"
            "    -v, --version         Show version\n"
            "  Config file: " << get_config_path() << "\n";
        return;
    }

    if (A.mode == "server") {
        std::cout <<
            "tinytcp server [options]\n"
            "  -p, --port <port>     Port to listen on (default from config)\n"
            "  -h, --help            Show this help\n";
        return;
    }

    if (A.mode == "client") {
        std::cout <<
            "tinytcp client [host] [options]\n"
            "  -m, --message <msg>   Message to send (default from config)\n"
            "  -p, --port <port>     Port (default from config)\n"
            "  -h, --help            Show this help\n";
        return;
    }
}

void print_version() {
    std::cout << "tinytcp " << VERSION << "\n";
}

