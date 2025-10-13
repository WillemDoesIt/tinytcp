#include "cli.hpp"
#include <iostream>


Args parse_args(int argc, char** argv, const Config& cfg) {
    Args args{};
    args.port = cfg.port;
    args.message = cfg.message;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") args.help = true;
        else if (arg == "--version" || arg == "-v") args.version = true;
        else if (arg == "server") args.server = true;
        else if (arg == "client") args.server = false;
        else if (arg == "--port" && i+1 < argc) args.port = std::stoi(argv[++i]);
        else if (arg == "--message" && i+1 < argc) args.message = argv[++i];
        else if (!args.server_ip.size()) args.server_ip = arg;
    }
    return args;
}

void print_help() {
    std::cout <<
    "Usage:\n"
    "  ttcp server [--port <num>]\n"
    "  ttcp client <ip> [--message <msg>] [--port <num>]\n\n"
    "Options:\n"
    "  -h --help        Show this help message\n"
    "  -v --version     Show version info\n"
    "  --port <num>     Override port (default from config)\n"
    "  --message <msg>  Override default message\n"
    "\n"
    "[!] Note: If listening stalls, your firewall or network may be blocking the port.";
}
