#pragma once

#include "client.h"
#include "proxy_server.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>

#include <iostream>
#include <vector>
#include <string>

#define OPEN_MAX 100
#define INFTIM -1

class PollManager
{
private:
    int sockfd;
    std::vector <pollfd> clientsfd;
    std::vector <Client *> clients;

public:
    PollManager(int sockfd);
    
    void addClient(Client* client);
    void Poll();

};