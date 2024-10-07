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

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.data()); // OR INADDR_LOOPBACK

    addrlen = sizeof(addr);

    // Error list in manual
    // https://man7.org/linux/man-pages/man2/bind.2.html
    if (bind(sockfd, (sockaddr*) &addr, addrlen) == -1) {
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
    sockaddr_in cliaddr;
    socklen_t cliaddrlen = sizeof(cliaddr);

    // Error list in manual
    // https://man7.org/linux/man-pages/man2/accept.2.html
    int clientfd = accept(sockfd, (sockaddr*)&cliaddr, &cliaddrlen);

    if (clientfd  == -1) {
        throw std::runtime_error("accept error ()");
    }

    std::string ip = inet_ntoa(cliaddr.sin_addr);
    
    // Сохраняем все данные клиента (структура Client)
    clients[clientfd].ip = ip;

    std::cout << "NEW CONNECT: " << clientfd  << " port" << std::endl;

    return clientfd; 
}

int ProxyServer::Poll()
{
    
    static size_t i = 0, maxi = 0;

    // Error list in manual
    // https://man7.org/linux/man-pages/man2/poll.2.html
    int nready = poll(poll_client, maxi + 1, INFTIM);

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

        return NO_ACTIVE_CLIENTS;
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

    return !NO_ACTIVE_CLIENTS;
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

    std::string res_query;
    char buf[1024] = {0};

    // Error list in manual
    // https://man7.org/linux/man-pages/man2/recvfrom.2.html
    // Connection reset by peer
    ssize_t bytes_read = recv(sockfd, buf, sizeof(buf), 0);

    if (bytes_read > 0) {
        //res_query = string(buf);
        //res_query.append(buf, sizeof(buf));
    }
    else if (bytes_read == 0) {
        close(sockfd);
    }
    else {
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

std::string ProxyServer::getServerIP() const
{
    return inet_ntoa(addr.sin_addr);
}

int ProxyServer::getServerFd() const
{
    return sockfd;
}
