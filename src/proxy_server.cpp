#include "../include/proxy_server.h"

ProxyServer::ProxyServer()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // System error:
    // Operation not permitted
    if (sockfd == INVALID_DESC) {
        throw std::system_error(errno, std::generic_category());
    }

    // Дескриптор сервера для принятия подключений
    poll_client[0].fd = sockfd;
    poll_client[0].events = POLLIN;

    for (size_t i = 1; i < OPEN_MAX; ++i) {
        poll_client[i].fd = -1;
    }
}

void ProxyServer::Start(int port, std::string ip)
{
    if (!isOpen()) {
        throw std::runtime_error("connect error (socket is closed)");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.data()); // OR INADDR_LOOPBACK

    addrlen = sizeof(server_addr);

    // Error list in manual
    // https://man7.org/linux/man-pages/man2/bind.2.html
    if (bind(sockfd, (sockaddr*) &server_addr, addrlen) == -1) {
        throw std::system_error(errno, std::generic_category());
    }

    // Error list in manual
    // https://man7.org/linux/man-pages/man2/listen.2.html
    if (listen(sockfd, SOMAXCONN) == -1) {
        throw std::system_error(errno, std::system_category());
    }
}

int ProxyServer::Accept()
{
    // Создание нового клиента
    sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);

    // Error list in manual
    // https://man7.org/linux/man-pages/man2/accept.2.html
    int clientfd = accept(sockfd, (sockaddr*)&client_addr, &addrlen);

    if (clientfd  == -1) {
        throw std::runtime_error("accept error");
    }

    std::string ip = inet_ntoa(client_addr.sin_addr);

    clients[clientfd].ip = ip;
    clients[clientfd].fd = clientfd;

    std::cout << "NEW CONNECT: port " << clientfd 
              << " ip: " << ip << std::endl;
            
    return clientfd; 
}

int ProxyServer::Poll()
{
    
    static size_t i = 0, maxi = 0;

    // Error list in manual
    // https://man7.org/linux/man-pages/man2/poll.2.html
    int nready = poll(poll_client, maxi + 1, TIMEOUT);

    if (nready == -1) {
        throw std::system_error(errno, std::system_category());
    }

    // Подключение новых клиентов
    if (poll_client[0].revents & POLLIN) {        
        for (i = 1; i < OPEN_MAX; ++i) {
            if (poll_client[i].fd < 0) {
                poll_client[i].fd = Accept();
                break;
            }
        }

        if (i == OPEN_MAX)
            throw std::runtime_error("poll error (too many clients)");
        
        poll_client[i].events = POLLIN;
        maxi = (i > maxi) ? i : maxi;

        return NO_EVENTS;
    }

    // Проверка всех клиентов на наличие данных 
    for (i = 1; i <= maxi; ++i) {

        if (poll_client[i].fd < 0)
            continue;
        
        // Клиент отключился
        if (poll_client[i].revents & (POLLHUP | POLLNVAL)) {
            clients.erase(poll_client[i].fd);
            poll_client[i].fd = -1;
        }

        if (poll_client[i].revents & POLLIN) {
            clients[poll_client[i].fd].active = true;
        } else {
            clients[poll_client[i].fd].active = false;
        }
    }

    return !NO_EVENTS;
}

void ProxyServer::setActiveClients(std::vector <Client>& v)
{
    v.clear();
    for (const auto& [sockfd, client] : clients) {
        if (client.active)
            v.push_back(client);
    }
}

void ProxyServer::Send(int sockfd, const std::string& massage)
{
    if (!isOpen()) {
        throw std::runtime_error("send error (socket is closed)");
    }

    // Error list in manual
    // https://man7.org/linux/man-pages/man2/send.2.html
    // Connection reset by peer
    ssize_t bytes_write = send(sockfd, massage.data(), massage.size(), 0);

    if (bytes_write < 0) {
        throw std::system_error(errno, std::generic_category());
    }
}

std::string ProxyServer::Recv(int sockfd)
{
    if (!isOpen()) {
        throw std::runtime_error("recv error (socket is closed)");
    }
    
    char buf[1024] {0};

    // Error list in manual
    // https://man7.org/linux/man-pages/man2/recvfrom.2.html
    // Connection reset by peer
    ssize_t bytes_read = recv(sockfd, buf, sizeof(buf), 0);

    if (bytes_read == 0) {
        close(sockfd);
    }
    else if (bytes_read == -1) {
        throw std::system_error(errno, std::generic_category());
    }

    return buf;
}

ProxyServer::~ProxyServer()
{
    if (isOpen()) {
        Close();
    }
}

void ProxyServer::Close()
{
    if (isOpen()) {
        close(sockfd);
        sockfd = INVALID_DESC;
    }  
}

bool ProxyServer::isOpen() const
{
    if (sockfd == INVALID_DESC) {
        return false;
    }

    return true;
}