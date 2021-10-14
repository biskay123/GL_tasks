#pragma once
#ifndef CLIENT_H
#define CLIENT_H
#include "includes.h"

class Client {
public:
    int16_t Answer();
    Client();
    ~Client();
private:
    int16_t Receive();
    int16_t Connect();
    char Buffer[BUFFER_SIZE] = { '\0' };
    struct {
#ifdef _WIN32
        WORD version_;
        SOCKADDR_IN addr_;
        WSADATA wsa_data_;
        SOCKET connection_socket_ = INVALID_SOCKET;
#elif __linux__
        int32_t connection_socket_ = 0;
#endif
        int16_t result_ = 0;
    }ConnectionSocket;
};
#endif // CLIENT_H
