#pragma once
#ifndef INCLUDES_H
#define INCLUDES_H

#ifdef _MSC_VER
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <cctype>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#endif //_MSC_VER

#define DEBUG
#define INFO
#define EMPTY_DIR 92
#define BUFFER_SIZE 512
#define DEFAULT_PORT 54000
#define SIGNED_TO_UNSIGNED_MISMATCH 0x80000000
#define ONE_MODEL_CLIENTS_AMOUNT 10

#ifdef __linux__
#define SOCKET_ERROR -1
#define INVALID_SOCKET ~0
#define WSAGetLastError() errno
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif //__linux__

#ifdef _WIN32
#include <WS2tcpip.h>
#include <windows.h>
#endif //  _WIN32

#include <stdint.h>
#include <string>
#include <cstring>
#include <array>
#include <vector>
#include <thread>
#include <mutex>
#include <cwctype>
#include <iostream>
#include <exception>

#endif // INCLUDES_H
