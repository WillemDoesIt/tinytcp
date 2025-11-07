#include "tcp.hpp"
#include <iostream>

int main() {
    init_sockets();

    std::cout << "Local IPv4: " << get_local_ipv4() << "\n";

    int mode = 0;
    std::cout << "1 = server, 2 = client: ";
    std::cin >> mode;

    if (mode == 1) {
        int port;
        std::cout << "Port: ";
        std::cin >> port;
        run_server(port);
    } else if (mode == 2) {
        std::string host;
        int port;
        std::string msg;

        std::cout << "Host: ";
        std::cin >> host;
        std::cout << "Port: ";
        std::cin >> port;
        std::cout << "Msg: ";
        std::cin.ignore();
        std::getline(std::cin, msg);

        run_client(host, port, msg);
    }

    cleanup_sockets();
    return 0;
}
