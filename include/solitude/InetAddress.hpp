#pragma once
#include <cstdint>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
namespace solitude{
    class InetAddress{
    private:
        sockaddr_in addr_{};
    public:
        InetAddress(const char* ip,uint16_t port);
        InetAddress()=default;
        ~InetAddress()=default;
        sockaddr* getAddr();
        const sockaddr* getAddr() const;
        socklen_t* getAddrLen();

        std::string getIp() const;
        uint16_t getPort() const;
    };
}