#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #pragma comment(lib, "ws2_32.lib")
  using socklen_t = int;
  #define CLOSESOCKET closesocket
  void init_sockets() { WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa); }
  void cleanup_sockets() { WSACleanup(); }

  int inet_pton_win(int af, const char* src, void* dst) {
      return InetPtonA(af, src, dst);
  }
  #define inet_pton inet_pton_win
#else
  #include <arpa/inet.h>
  #include <unistd.h>
  #define SOCKET int
  #define CLOSESOCKET close
  void init_sockets() {}
  void cleanup_sockets() {}
#endif

#include <iostream>
#include <cstring>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [server|client] [host]\n";
        return 1;
    }

    init_sockets();
    const int PORT = 49153;

    if (std::strcmp(argv[1], "server") == 0) {
        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(PORT);
        bind(sock, (sockaddr*)&addr, sizeof(addr));
        listen(sock, 1);
        std::cout << "Server listening on port " << PORT << "...\n";

        SOCKET client = accept(sock, nullptr, nullptr);
        char buf[1024]{};
        int bytes = recv(client, buf, sizeof(buf)-1, 0);
        if (bytes > 0) std::cout << "Received: " << buf << "\n";
        CLOSESOCKET(client);
        CLOSESOCKET(sock);
    } 
    else if (std::strcmp(argv[1], "client") == 0) {
        const char* host = (argc > 2) ? argv[2] : "127.0.0.1";
        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        inet_pton(AF_INET, host, &addr.sin_addr);

        if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == 0) {
            const char* msg = "Hello from client!";
            send(sock, msg, strlen(msg), 0);
            std::cout << "Message sent to " << host << ":" << PORT << "\n";
        } else {
            std::cerr << "Failed to connect to " << host << "\n";
        }
        CLOSESOCKET(sock);
    } 

    cleanup_sockets();
}

