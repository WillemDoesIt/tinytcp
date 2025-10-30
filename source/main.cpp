#ifdef _WIN32
  #include <winsock2.h>
  #pragma comment(lib, "ws2_32.lib")
  using socklen_t = int;
  #define CLOSESOCKET closesocket
  void init_sockets() { WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa); }
  void cleanup_sockets() { WSACleanup(); }
#else
  #include <arpa/inet.h>
  #include <unistd.h>
  #define SOCKET int
  #define CLOSESOCKET close
  void init_sockets() {}
  void cleanup_sockets() {}
#endif

#include <iostream>

int main() {
    init_sockets();

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(49153);

    bind(sock, (sockaddr*)&addr, sizeof(addr));
    listen(sock, 1);

    std::cout << "Listening on port 49153..." << std::endl;

    SOCKET client = accept(sock, nullptr, nullptr);
    std::cout << "Client connected!" << std::endl;

    CLOSESOCKET(client);
    CLOSESOCKET(sock);
    cleanup_sockets();
}

