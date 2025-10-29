#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(49153);

    bind(sock, (sockaddr*)&addr, sizeof(addr));
    listen(sock, 1);

    std::cout << "Listening on port 49153..." << std::endl;

    int client = accept(sock, nullptr, nullptr);
    std::cout << "Client connected!" << std::endl;

    pause();
    close(client);
    close(sock);
}

