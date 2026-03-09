#pragma once
#include <netinet/in.h>
#include <string>

namespace eirian {
    class InetAddress{
    private:
        sockaddr_in addr_{};
        socklen_t addr_len_{sizeof(sockaddr_in)};
    public:
        InetAddress(const std::string& ip,uint16_t port);
        InetAddress()=delete;
        ~InetAddress()=default;
        sockaddr* getAddr();
        [[nodiscard]] const sockaddr* getAddr() const;
        socklen_t* getAddrLen();
        [[nodiscard]] socklen_t getAddrLen()const;

        [[nodiscard]] std::string getIp() const;
        [[nodiscard]] uint16_t getPort() const;
    };
}  // namespace eirian
