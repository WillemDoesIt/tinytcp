#include "toml.hpp"
#include "tcp.hpp"
#include "args.hpp"

#include <iostream>

int main(int argc, char** argv) {
    Args args;
    try { args = parse_args(argc, argv); }
    catch (const std::exception& e) { std::cerr << e.what() << "\n"; return 1; }

    if (args.help || args.help_verbose) {
        print_help(args);
        return 0;
    } else if (args.version) {
        print_version();
        return 0;
    }

    const Config CONFIG = load_config();
    const int PORT = (args.port != -1) ? args.port : CONFIG.port;
    const std::string MESSAGE = !args.message.empty() ? args.message : CONFIG.message;

    init_sockets();

    if (args.mode == "server") run_server(PORT);
    else if (args.mode == "client") run_client(args.host, PORT, MESSAGE);
    else { std::cerr << "Unknown mode: " << args.mode << "\n"; return 1; }

    cleanup_sockets();
}

