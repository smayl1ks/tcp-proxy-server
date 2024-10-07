#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>

#include <stdexcept>
#include <system_error>

#include <string>

// Значение timeout
// INFTIM - ждать события
#define INFTIM -1

class Client 
{
private:
    constexpr static int INVALID_DESC = -1;

    int sockfd = INVALID_DESC;
    struct sockaddr_in servaddr;
    socklen_t addrlen;

public:

    Client();
    ~Client();

    void Connect(int port, std::string ip);

    void Send(std::string query);
    std::string Recv();

    void Close();

    std::string getServerIP() const;
    int getClientFd() const;

    bool isOpen() const;
    bool isConnect() const;
};