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

// Максимальное число клиентов в poll
constexpr static int OPEN_MAX = 100;
// Значение timeout (пока не пройзойдет событие)
constexpr static int TIMEOUT = -1;
// Флаг: На сервере нет событий
constexpr static int NO_EVENTS = 1;

constexpr static int INVALID_DESC = -1;

struct Client
{
    bool active = false;
    int fd;
    std::string ip;
    // ...
};

class ProxyServer
{
private:
    int sockfd = INVALID_DESC;

    struct pollfd poll_client[OPEN_MAX];
    struct sockaddr_in server_addr;
    socklen_t addrlen;

    // Файловые дескрипторы и данные клиетов 
    std::unordered_map <int, Client> clients;

public:


    ProxyServer();
    ~ProxyServer();

    void Start(int port, std::string ip);
    int  Poll();
    int  Accept();

    void Send(int sockfd, const std::string& massage);
    std::string Recv(int sockfd);

    void Close();
    void setActiveClients(std::vector <Client>& client);
    bool isOpen() const;
};