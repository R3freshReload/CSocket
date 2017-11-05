//
// Created by Daniel S on 01.11.2017.
//

#ifndef SOCKET_SOCKET_H
#define SOCKET_SOCKET_H

// To initialize WSA (Has not been tested with anything but MINGW yet)
#define COMPWIN

// For inet_ntop under MINGW
#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WIN8

#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>

#include <rpc.h>
#include <ws2tcpip.h>   // IP_HDRINCL

#include <unistd.h>
#include <iostream>
#include <string>

class Socket {
public:

    // Create Socket instance from host and port.
    // Autoconnect is for setting options before connection.
    Socket(std::string host, int port, bool autoconnect = true) {
#ifdef COMPWIN
        WSADATA wsock;
        if (WSAStartup(MAKEWORD(2,2),&wsock) != 0) {
            std::cerr << "WSAStartup() failed" << std::endl;
        }
#endif
        this->host = host;
        this->port = port;

        create();
        if (autoconnect)
            connect();
    }

    ~Socket() {
        this->close();
#ifdef COMPWIN
        WSACleanup();
#endif
    }

    // Create socket from socket number and client infos (Mainly for accepting sockets from a ServerSocket)
    Socket(int socket, SOCKADDR_IN client) {
        this->sock = socket;
        this->port = client.sin_port;

        struct in_addr ipAddr = client.sin_addr;
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);
        this->host = std::string(str);

        this->open = true;
    }

    // Sends a C-Style string
    void send(const char* st) {
        int got = ::send(sock, st, strlen(st), 0);
    }

    // Sends a C++-String
    void send(std::string s) {
        this->send(s.c_str());
    }

    // Returns false if connection was closed with socket->close()
    // or if autoconnect is false and connect hasn't been called yet
    bool isOpen() {
        return this->open;
    }

    // Probes if the socket is still connected.
    // Returns false if connection is not open.
    bool isConnected() {
        SOCKADDR add;
        int siz = sizeof(add);
        int ret = getpeername(this->sock, &add, &siz);
        return (this->open && ret != EAGAIN && ret != EBADF && ret != ENOTCONN);
    }

    // Set a socket option on a specific layer
    bool setOption(int option, int layer, int val) {
        int optlen = sizeof(option);
        if(setsockopt(this->sock, SOL_SOCKET, option, (const char*)&val, optlen) == SOCKET_ERROR) {
            return false;
        }
        return true;
    }

    // Set a socket option on the SOL_SOCKET layer
    bool setSocketOption(int option, int val) {
        setOption(option, SOL_SOCKET, val);
    }

    // Returns the value of an option on a specific layer
    int getOption(int option, int layer) {
        int val = 0;
        int optlen = sizeof(option);
        if(getsockopt(this->sock, SOL_SOCKET, option, (char*)&val, &optlen) == SOCKET_ERROR) {
            std::cerr << "Cannot get option value" << std::endl;
        }
        return val;
    }

    // Returns the value of an option on the SOL_SOCKET layer
    int getSocketOption(int option) {
        return getOption(option, SOL_SOCKET);
    }

    // Easy way to set Keepalive on socket
    void setKeepalive() {
        this->setSocketOption(SO_KEEPALIVE, 1);
    }

    // Reads until \n, \0, \r or the end of the buffer
    // and returns the string. Blocks if buffer is empty
    std::string readLine() {
        char* buff = new char[4096]{0};
        int am = recv(sock, buff, 4096 - 1, MSG_PEEK);
        std::string res = "";
        int i;
        for (i = 0; i < am; ++i) {
            char c = buff[i];
            if (c == '\n' || c == '\0' || c == '\r') break;
            res += c;
        }

        am = recv(sock, buff, i+1, 0);

        return res;
    }

    // Read into a buffer without flags
    void read(char* buffer, int len) {
        ::recv(sock, buffer, len, 0);
    }

    // Read into a buffer with flags
    void read(char* buffer, int len, int flags) {
        ::recv(sock, buffer, len, flags);
    }

    // Close the socket
    void close() {
        ::close(this->sock);
        this->open = false;
    }

    // Connects the socket to the specified host and port
    void connect() {
        SOCKADDR_IN dest;
        dest.sin_addr.s_addr = inet_addr(host.c_str());
        dest.sin_family = AF_INET;
        dest.sin_port = htons(port);

        if (::connect(sock, (SOCKADDR*) &dest, sizeof(dest)) == SOCKET_ERROR) {
            std::cerr << "Connection failed." << std::endl;
            return;
        }
        this->open = true;
    }
private:
    int sock;
    int port;
    std::string host;

    bool open = false;

    // Create a new socket (get socketfd from kernel)
    void create() {
        if((sock = ::socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR) {
            std::cerr << "Creation of socket failed." << std::endl;
            return;
        }
    }
};


class ServerSocket {
public:
    // Creates a new server socket on a specific port
    ServerSocket(int port) {
#ifdef COMPWIN
        WSADATA wsock;
        if (WSAStartup(MAKEWORD(2,2),&wsock) != 0) {
            std::cerr << "WSAStartup() failed" << std::endl;
        }
#endif

        this->port = port;

        create();
        bind();
        listen();
    }

    ~ServerSocket() {
        this->close();
#ifdef COMPWIN
        WSACleanup();
#endif
    }

    // Accepts a new connection. Blocks if none are queued.
    Socket* accept() {
        SOCKADDR_IN address;
        int addsize = sizeof(address);

        SOCKET retsock = 2;
        if ((retsock = ::accept(this->sock, (SOCKADDR *) &address, (socklen_t*)(&addsize))) == SOCKET_ERROR) {
            std::cerr << "Error when accepting server socket." << std::endl;
        }

        struct in_addr ipAddr = address.sin_addr;
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);

        Socket* ret = new Socket(retsock, address);
        return ret;
    }

    // Closes the server socket.
    // (None of the connections that were accepted though)
    void close() {
        ::close(sock);
        this->open = false;
    }

    // Returns false if connection was closed with socket->close()
    bool isOpen() {
        return this->open;
    }


    // Set a socket option on a specific layer
    bool setOption(int option, int layer, int val) {
        int optlen = sizeof(option);
        if(setsockopt(this->sock, SOL_SOCKET, option, (const char*)&val, optlen) == SOCKET_ERROR) {
            return false;
        }
        return true;
    }

    // Set a socket option on the SOL_SOCKET layer
    bool setSocketOption(int option, int val) {
        setOption(option, SOL_SOCKET, val);
    }

    // Returns the value of an option on a specific layer
    int getOption(int option, int layer) {
        int val = 0;
        int optlen = sizeof(option);
        if(getsockopt(this->sock, SOL_SOCKET, option, (char*)&val, &optlen) == SOCKET_ERROR) {
            std::cerr << "Cannot get option value" << std::endl;
        }
        return val;
    }

    // Returns the value of an option on the SOL_SOCKET layer
    int getSocketOption(int option) {
        return getOption(option, SOL_SOCKET);
    }

    // Easy way to set Keepalive on socket
    void setKeepalive() {
        this->setSocketOption(SO_KEEPALIVE, 1);
    }

private:
    int sock;
    int port;

    bool open = false;

    // Create a new socket (get socketfd from kernel)
    void create() {
        if ((this->sock = ::socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR) {
            std::cerr << "Creation of server socket failed." << std::endl;
            return;
        }
    }

    // Binds the socket to the specified port
    void bind() {
        SOCKADDR_IN address;

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(this->port);

        if (::bind(this->sock, (SOCKADDR*) &address, sizeof(address)) == SOCKET_ERROR) {
            std::cerr << "Cannot bind server socket." << std::endl;
            return;
        }
    }

    // Start to listen for incoming connections on the specified port
    void listen() {
        if (::listen(this->sock, 5) == SOCKET_ERROR) {
            std::cerr << "Cannot listen on port: " << this->port << std::endl;
            return;
        }
        this->open = true;
    }
};

#endif //SOCKET_SOCKET_H
