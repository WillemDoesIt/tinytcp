#include <iostream>
#include <thread>
#include "config.hpp"
#include "network.hpp"
#include "cli.hpp"

const std::string VERSION = "0.3.0-alpha";
const std::string PKG = "ttcp";

int main(int argc, char** argv) {
    cross_platform_socket::initialize();
    Config cfg = load_config(PKG);
    Args args = parse_args(argc, argv, cfg);

    if (args.help) { print_help(); return 0; }
    if (args.version) { std::cout << PKG + ": " + VERSION + "\n"; return 0; }

    if (args.server) { start_server(args.port); while (true) std::this_thread::sleep_for(std::chrono::hours(24)); }
    else {
        if (args.server_ip.empty()) { 
            std::cerr << "Client requires SERVER_IP argument\n"; 
            return 1; 
        }
        send_message(args.server_ip, args.port, args.message);
    }

    cross_platform_socket::cleanup();
    return 0;
}

