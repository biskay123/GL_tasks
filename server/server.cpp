#include "Includes.h"
#include "server.h"

Server::Server()
{
#ifdef __linux__
    int16_t ConnectSocket, ListenSocket;
    struct sockaddr_in addr;
    ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > ListenSocket) std::cerr<<"socket creating failed with error: " << std::to_string(errno)<<std::endl;
    LogMsg(0, "Socket created\n");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425 + UniquePortIndex);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    iResult = bind(ListenSocket, (struct sockaddr*)&addr, sizeof(addr));
    if (0 > iResult)std::cerr<<"binding failed with error: " << std::to_string(errno) << std::endl;
    iResult = listen(ListenSocket, 1);
    if (0 > iResult)std::cerr<<"listening failed with error: " << std::to_string(errno) << std::endl;
    ConnectSocket = accept(ListenSocket, NULL, NULL);
    if (0 > ConnectSocket)std::cerr<<"accept failed with error: " << std::to_string(errno) << std::endl;
#elif _WIN32
    // Initialze winsock
    Server::ConnectionSocket.version_ = MAKEWORD(2, 2);
    Server::ConnectionSocket.result_ = WSAStartup(Server::ConnectionSocket.version_, &Server::ConnectionSocket.wsa_data_);
    if (0 != Server::ConnectionSocket.result_) std::cerr<<"recv failed with error: " << std::to_string(Server::ConnectionSocket.result_) << std::endl;
    // fill structure
    Server::ConnectionSocket.addr_.sin_addr.S_un.S_addr = INADDR_ANY;
    Server::ConnectionSocket.addr_.sin_port = htons(DEFAULT_PORT);
    Server::ConnectionSocket.addr_.sin_family = AF_INET;
    // Create a socket
	Server::ConnectionSocket.connection_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (Server::ConnectionSocket.connection_socket_ == INVALID_SOCKET)
    {
        std::cerr << "Can't create a socket! Quitting" << std::endl;
        return;
    }
    bind(Server::ConnectionSocket.connection_socket_, (sockaddr*)&Server::ConnectionSocket.addr_, sizeof(Server::ConnectionSocket.addr_));

	// Tell Winsock the socket is for listening 
	listen(Server::ConnectionSocket.connection_socket_, SOMAXCONN);
	// Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);
	std::cout << "almst";
	Server::ConnectionSocket.accept_socket_ = accept(Server::ConnectionSocket.connection_socket_, (sockaddr*)&client, &clientSize);
	if(Server::ConnectionSocket.accept_socket_ == INVALID_SOCKET)
	std::cout << "im here";
	char host[NI_MAXHOST];		// Client's remote name
	char service[NI_MAXSERV];	// Service (i.e. port) the client is connect on

	ZeroMemory(host, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << host << " connected on port " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " <<
			ntohs(client.sin_port) << std::endl;
	}

	// Close listening socket
	closesocket(Server::ConnectionSocket.connection_socket_);

	Server::ConnectionSocket.result_ = SendReceive();

#endif
}

Server::~Server()
{
#ifdef __linux__
	close(Client::ConnectionSocket.connection_socket_);
#endif
#ifdef WIN32
	shutdown(Server::ConnectionSocket.accept_socket_, SD_SEND);
	closesocket(Server::ConnectionSocket.accept_socket_);
	WSACleanup();
#endif //WIN32
}

int16_t Server::SendReceive()
{
	// While loop: accept and echo message back to client
	char buf[4096];
	char bufff[3] = "Hi";
	send(Server::ConnectionSocket.accept_socket_, bufff, 3, 0);
	while (true)
	{
		ZeroMemory(buf, 4096);

		// Wait for client to send data
		int bytesReceivertemp = 0;
		///
		unsigned long l = 1;
		///
		while (true)
		{
			ioctlsocket(Server::ConnectionSocket.accept_socket_, FIONREAD, &l);
			if (l == 0)
				break;
			bytesReceivertemp += recv(Server::ConnectionSocket.accept_socket_, buf + bytesReceivertemp, 4, 0);
		}
		if (bytesReceivertemp == SOCKET_ERROR)
		{
			std::cerr << "Error in recv(). Quitting" << std::endl;
			break;
		}

		if (bytesReceivertemp > 0)
		{
			std::cout << std::string(buf, 0, bytesReceivertemp) << std::endl;
			send(Server::ConnectionSocket.accept_socket_, buf, bytesReceivertemp + 1, 0);

		}


	}
	return Server::ConnectionSocket.result_;
}
