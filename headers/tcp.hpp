// -----------------------------------------------------------------------
// 
// tcp.hpp v0.4.0-beta
// https://github.com/WillemDoesIt/tinytcp
// SPDX-License-Identifier: Unlicense
//
// -----------------------------------------------------------------------
//
// Cross-platform TCP helper library (header-only)
// See examples/ and README.md for more details.
//
// -----------------------------------------------------------------------
//
// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
// 
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// 
// For more information, please refer to <https://unlicense.org/>
//
// -----------------------------------------------------------------------

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #pragma comment(lib, "ws2_32.lib")
  
  using socklen_t = int;
  #define CLOSESOCKET closesocket
  
  /**
   * @brief Initializes the Windows socket library (Winsock).
   *
   * This function must be called once before any socket operations
   * (such as creating sockets, connecting, sending, or receiving data)
   * when running on Windows. On non-Windows platforms, this function
   * is a no-op.
   *
   * @note Failing to call this on Windows will cause socket operations to fail.
   */
  void init_sockets() { 
      WSADATA wsa; 
      WSAStartup(MAKEWORD(2,2), &wsa); 
  }
  
  /**
   * @brief Cleans up the Windows socket library.
   *
   * This function should be called once after all socket operations are complete.
   * On non-Windows platforms, this function is a no-op.
   */
  void cleanup_sockets() { 
      WSACleanup(); 
  }
  
  /**
   * @brief Cross-platform wrapper for inet_pton on Windows.
   */
  int inet_pton_win(int af, const char* src, void* dst) { 
      return InetPtonA(af, src, dst); 
  }
  #define inet_pton inet_pton_win
  
#else
  #include <arpa/inet.h>
  #include <unistd.h>
  #include <netdb.h>
  
  #define SOCKET int
  #define CLOSESOCKET close
  
  void init_sockets() {}
  void cleanup_sockets() {}
#endif

#include <iostream>
#include <cstring>

/**
 * @brief Returns the host machine's primary IPv4 address.
 *
 * This function determines the local IPv4 address by creating a temporary
 * UDP socket and connecting to a well-known public IP address (Google DNS 8.8.8.8).
 * Note that no data is actually sent; this technique simply allows the OS
 * to determine the appropriate interface for outgoing traffic, which is then
 * queried using `getsockname`.
 *
 * @return A string containing the local IPv4 address, or "unknown" if the
 *         address could not be determined.
 *
 * @example
 * init_sockets();
 * std::string local_ip = get_local_ipv4();
 * std::cout << "Local IPv4: " << local_ip << std::endl;
 * cleanup_sockets();
 */
static std::string get_local_ipv4() {
    const SOCKET SOCK = socket(AF_INET, SOCK_DGRAM, 0);
    if (SOCK < 0) return "unknown";

    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(53);
    inet_pton(AF_INET, "8.8.8.8", &serv.sin_addr);

    if (connect(SOCK, reinterpret_cast<const sockaddr*>(&serv),
                sizeof(serv)) < 0) {
        CLOSESOCKET(SOCK);
        return "unknown";
    }

    sockaddr_in name{};
    socklen_t namelen = sizeof(name);

    if (getsockname(SOCK, reinterpret_cast<sockaddr*>(&name),
                    &namelen) < 0) {
        CLOSESOCKET(SOCK);
        return "unknown";
    }

    char buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &name.sin_addr, buf, sizeof(buf));

    CLOSESOCKET(SOCK);
    return std::string(buf);
}


/**
 * @brief Runs a simple TCP server on the specified port.
 *
 * The server listens on all interfaces and accepts a single client connection.
 * Once a client connects, the server reads a single message from the client
 * and prints it to the standard output. The server then closes the connection
 * and stops listening.
 *
 * @param port The port number to listen on.
 *
 * @note This is a blocking call: it will wait indefinitely until a client
 *       connects. For production code, consider using non-blocking sockets
 *       or multithreading to handle multiple clients.
 *
 * @example
 * init_sockets();
 * run_server(50001); // Waits for client to connect and receive message
 * cleanup_sockets();
 */
void run_server(int port) {
    const SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket < 0) return;

    const sockaddr_in server_addr = [port]() {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);
        return addr;
    }();


    bind(listen_socket, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(listen_socket, 1);

    std::cout << "Server listening on " << get_local_ipv4() << ":" << port << "...\n";

    const SOCKET client_socket = accept(listen_socket, nullptr, nullptr);
    char recv_buffer[1024]{};
    int bytes_received = recv(client_socket, recv_buffer, sizeof(recv_buffer) - 1, 0);

    if (bytes_received > 0) std::cout << "Received: " << recv_buffer << "\n";

    CLOSESOCKET(listen_socket);
}


/**
 * @brief Connects to a TCP server and sends a message.
 *
 * Attempts to connect to the server at the specified host and port.
 * If the connection is successful, the message is sent over the socket. 
 * Upon completion, the socket is closed.
 *
 * @param host The IPv4 address of the server (e.g., "10.0.0.191").
 * @param port The TCP port number on which the server is listening.
 * @param msg  The message string to send.
 *
 * @note This is a blocking call: it waits until the connection is established
 *       or fails. No retries are performed.
 *
 * @example
 * init_sockets();
 * run_client("10.0.0.191", 50001, "Hello from client");
 * cleanup_sockets();
 */
void run_client(const std::string& server_ip, int server_port, const std::string& message) {
    const SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) return;

    const sockaddr_in server_addr = [server_ip, server_port]() {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(server_port);
        inet_pton(AF_INET, server_ip.c_str(), &addr.sin_addr);
        return addr;
    }();

    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == 0) {
        send(client_socket, message.c_str(), message.size(), 0);
        std::cout << "Message sent to " << server_ip << ":" << server_port << "\n";
    } else {
        std::cerr << "Failed to connect to " << server_ip << ":" << server_port << "\n";
    }

    CLOSESOCKET(client_socket);
}
