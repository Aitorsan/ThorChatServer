#ifndef CLIENT_INFO_HPP
#define CLIENT_INFO_HPP
#include <string>
#include <unistd.h>
#include <fcntl.h>

struct ClientInfo
{
    std::string name;
    std::string address;
    int socketFd;
    int port;
    bool isClientConnected()
    {
        return ( fcntl(socketFd, F_GETFD) != -1 || errno != EBADF);
    }
    bool operator==(const ClientInfo& other) const
    {
        return other.name == this->name && other.address == this->address
               && other.socketFd == this->socketFd && other.port == this->port;
    }
};

#endif /*CLIENT_INFO_HPP*/