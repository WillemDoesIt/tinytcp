#include "tcp.hpp"
#include <iostream>
#include <thread>
#include <chrono>

// A tiny echo client that connects, sends a line, waits, then sends another.

int main() {
    init_sockets();

    std::string host = "127.0.0.1";
    int port = 50001;

    for (int i = 1; i <= 2; i++) {
        std::string msg = "ping " + std::to_string(i);
        std::cout << "sending: " << msg << "\n";
        run_client(host, port, msg);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    cleanup_sockets();
    return 0;
}
