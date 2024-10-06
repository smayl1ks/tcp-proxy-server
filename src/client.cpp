#include "../include/client.h"

Client::Client()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // System error:
    // Operation not permitted
    if (sockfd == INVALID_DESC) {
        throw std::system_error(errno, std::generic_category());
    }
}

void Client::Connect(int port, std::string ip)
{
    if (!isOpen()) {
        throw std::runtime_error("connect error (socket is closed)");
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(ip.data());

    addrlen = sizeof(servaddr);

    // Error list:
    // Connection refused
    // Connection timed out
    // Socket is already connected
    if (connect(sockfd, (struct sockaddr *) &servaddr, addrlen) == -1) {
        throw std::system_error(errno, std::generic_category());
    }

}

void Client::Send(std::string query)
{
    if (!isOpen()) {
        throw std::runtime_error("send error (socket is closed)");
    }

    // Error list in manual
    // https://man7.org/linux/man-pages/man2/send.2.html
    ssize_t bytes_write = send(sockfd, query.data(), query.size(), 0);

    if (bytes_write < 0) {
        throw std::system_error(errno, std::generic_category());
    }
       
}

std::string Client::Recv()
{
    if (!isOpen()) {
        throw std::runtime_error("recv error (socket is closed)");
    }

    std::string res_query;
    char buf[1024] = {0};

    // Error list in manual
    // https://man7.org/linux/man-pages/man2/recvfrom.2.html
    ssize_t bytes_read = recv(sockfd, buf, sizeof(buf), 0);
    
    if (bytes_read > 0)
    {
        res_query.append(buf, sizeof(buf));
    } 
    else if (bytes_read == 0)
    {
        return "DISCONNECTED";
    } 
    else 
    {
        throw std::system_error(errno, std::generic_category());
    }

    return res_query;
}

Client::~Client()
{
    if (isOpen()) {
        Close();
    }
}

void Client::Close()
{
    if (isOpen()) {
        close(sockfd);
        sockfd = INVALID_DESC;
    }  
}

bool Client::isOpen() const
{
    if (sockfd == INVALID_DESC) {
        return false;
    }

    return true;
}

bool Client::isConnect() const
{
    return isOpen();
}

std::string Client::getServerIP() const
{
    return inet_ntoa(servaddr.sin_addr);
}

int Client::getClientFd() const
{
    return sockfd;
}
