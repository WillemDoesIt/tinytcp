#include "toml.hpp"
#include "tcp.hpp"
#include "args.hpp"

#include <iostream>

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

