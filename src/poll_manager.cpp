#include "../include/poll_manager.h"

PollManager::PollManager(int serverfd) : sockfd(serverfd)
{
    clientsfd.resize(OPEN_MAX);
    clientsfd[0].fd = serverfd;
    clientsfd[0].events = POLLIN;

    for (size_t i = 1; i < OPEN_MAX; i++) {
        clientsfd[i].fd = -1;
    }
}

void PollManager::addClient(Client* client) 
{
    size_t i;
    for (i = 1; i < OPEN_MAX; ++i) {
        if (clientsfd[i].fd < 0) {
            clientsfd[i].fd = client->getClientFd();
            clients.push_back(client);
            break;
        }
    }

    if (i == OPEN_MAX) {
        throw std::runtime_error("add client error (Too many client)");
    }

    clientsfd[i].events = POLLIN;
}

void PollManager::Poll()
{
    size_t maxi = clients.size();

    // Error list in manual
    int nready = poll(clientsfd.data(), maxi + 1, INFTIM);

    if (nready == -1) {
        throw std::runtime_error("poll error ()");
    }

    // Обработка нового подключения
    if (clientsfd[0].revents & POLLIN) {
        // Обработка нового подключения
        ProxyServer* server = static_cast<Server*>(this);
        server->handleNewConnection(server->getListenFd());
        nready--;
    }
}