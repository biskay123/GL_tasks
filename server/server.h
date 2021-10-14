#pragma once
#ifndef SERVER_H
#define SERVER_H
#include "Includes.h"

class Server {
public:
public:
    Server();
    ~Server();
private:
    int16_t SendReceive();
    char Buffer[BUFFER_SIZE] = { '\0' };
    struct {
#ifdef _WIN32
        WORD version_;
        SOCKADDR_IN addr_;
        WSADATA wsa_data_;
        SOCKET accept_socket_;
        SOCKET connection_socket_ = INVALID_SOCKET;
#elif __linux__
        int32_t connection_socket_ = 0;
#endif
        int16_t result_ = 0;
    }ConnectionSocket;
};

#endif SERVER_H