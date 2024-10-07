#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <system_error>

#include <string>
#include <vector>
#include <unordered_map>

// Максимальное число клиентов
#define OPEN_MAX 100

// Флаг: клиентов для обработки запросов нет
#define NO_ACTIVE_CLIENTS 1

// Значение timeout
// Ждать пока не пройзойдет событие
#define INFTIM -1

struct Client
{
    bool active = false;
    std::string ip;
    // ...
};

class ProxyServer
{
private:
    constexpr static int INVALID_DESC = -1;

    int sockfd = INVALID_DESC;
    struct sockaddr_in addr;
    socklen_t addrlen;

    struct pollfd poll_client[OPEN_MAX];

public:

    // Файловые дескрипторы и данные клиентов
    std::unordered_map <int, Client> clients;

    ProxyServer();
    ~ProxyServer();

    void Start(int port, std::string ip);
    int  Poll();
    int  Accept();

    void Send(int sockfd, const std::string& massage);
    std::string Recv(int sockfd);

    void Close();

    std::string getServerIP() const;
    int getServerFd() const;

    bool isOpen() const;
};