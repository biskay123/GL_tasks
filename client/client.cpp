#include "client.h"
#include "Includes.h"

Client::Client() {
#ifdef __linux__
    Client::ConnectionSocket.connection_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > Client::ConnectionSocket.connection_socket_) std::cerr << "socket creating failed with error: " << Client::ConnectionSocket.result_ << std::endl;
    Client::ConnectionSocket.addr_.sin_family = AF_INET;
    Client::ConnectionSocket.addr_.sin_port = htons(3425);
    Client::ConnectionSocket.addr_.sin_addr.s_addr = htonl(INADDR_ANY);
#elif _WIN32
    //Initialize WinSock
    Client::ConnectionSocket.version_ = MAKEWORD(2, 2); // generating the versioning word expected by WSAStartup
    Client::ConnectionSocket.result_ = WSAStartup(Client::ConnectionSocket.version_, &Client::ConnectionSocket.wsa_data_); // returns zero if success. otherwise error code
    if (0 != Client::ConnectionSocket.result_)
    {
        std::cerr << "Can't start Winsock, Err #" << Client::ConnectionSocket.result_ << std::endl;
        return;
    }
    //fill in a sockaddr_in structure 
    std::string ipAddress = "127.0.0.1";
    //Client::ConnectionSocket.addr_.sin_addr.s_addr = inet_addr("127.0.0.1");
    Client::ConnectionSocket.addr_.sin_port = htons(DEFAULT_PORT);
    Client::ConnectionSocket.addr_.sin_family = AF_INET;
    inet_pton(AF_INET, ipAddress.c_str(), &Client::ConnectionSocket.addr_.sin_addr);
#endif
    Client::ConnectionSocket.result_ = Connect();
}

int16_t Client::Connect() {
#ifdef __linux__
    int32_t ConnectionSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > connect(Client::ConnectionSocket.connection_socket_, (struct sockaddr*)&Client::ConnectionSocket.addr_, sizeof(Client::ConnectionSocket.addr_)))std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
#elif _WIN32
    Client::ConnectionSocket.connection_socket_ = socket(AF_INET, SOCK_STREAM, NULL);
    Client::ConnectionSocket.result_ = connect(Client::ConnectionSocket.connection_socket_, (SOCKADDR*)&Client::ConnectionSocket.addr_, sizeof(Client::ConnectionSocket.addr_));
    if (Client::ConnectionSocket.result_ == SOCKET_ERROR)
    {
        std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
        closesocket(Client::ConnectionSocket.connection_socket_);
        WSACleanup();
        return 0;
    }
#endif
    while (1) Client::ConnectionSocket.result_ = Receive();
    return Client::ConnectionSocket.result_;
}

int16_t Client::Receive() {
    while (true) {
        SecureZeroMemory(this->Buffer, BUFFER_SIZE);
        Client::ConnectionSocket.result_ = recv(Client::ConnectionSocket.connection_socket_, this->Buffer, BUFFER_SIZE, 0);
#ifdef __linux__
        if (-1 == Client::ConnectionSocket.result_) std::cerr<<"sending failed with error: " << std::to_string(errno)<<std::endl;
#elif _WIN32
        if (Client::ConnectionSocket.result_ > 0)
        {
            // Echo response to console
            std::cout << "SERVER> " << std::string(this->Buffer, 0, Client::ConnectionSocket.connection_socket_) << std::endl;
        }
#endif
        Client::ConnectionSocket.result_ = Answer();
    }
    return Client::ConnectionSocket.result_;
}

int16_t Client::Answer() {
    std::string userInput;
    do
    {
        // Prompt the user for some text
        std::cout << "> ";
        std::getline(std::cin, userInput);

        if (userInput.size() > 0)		// Make sure the user has typed in something
        {
            // Send the text
            int sendResult = send(Client::ConnectionSocket.connection_socket_, userInput.c_str(), userInput.size() + 1, 0);
            if (sendResult != SOCKET_ERROR)
            {
                Client::Receive();
            }
        }

    } while (userInput.size() > 0);
    return Client::ConnectionSocket.result_;
}

Client::~Client() {
#ifdef __linux__
    close(Client::ConnectionSocket.connection_socket_);
#endif
#ifdef WIN32
    shutdown(Client::ConnectionSocket.connection_socket_, SD_SEND);
    closesocket(Client::ConnectionSocket.connection_socket_);
    WSACleanup();
#endif //WIN32
}

