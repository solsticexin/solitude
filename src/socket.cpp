#include "eirian/socket.hpp"
#include "eirian/InetAddress.hpp"
#include "eirian/logger.hpp"


#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <optional>
#include <system_error>

namespace eirian {
    Socket::Socket() {
    socket_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ == -1) {
        getLogger()->error("create socket failed: {}", std::strerror(errno));
        throw std::system_error(errno,std::generic_category(),"create socket_fd failed");
    }
    getLogger()->info("socket created, fd={}", socket_fd_);
}
    Socket::Socket(const int fd):socket_fd_(fd){}
    Socket::~Socket(){
        if (socket_fd_ != -1) {
            close(socket_fd_);
            socket_fd_ = -1;
        }
    }
    //允许移动构造和移动赋值
    Socket::Socket(Socket&& other) noexcept: socket_fd_(other.socket_fd_) {
        other.socket_fd_ = -1;
    }
    Socket& Socket::operator = (Socket&& other)noexcept{
        if (this != &other) {
            if (this->socket_fd_ != -1) {
                close(this->socket_fd_);
            }
            this->socket_fd_ =other.socket_fd_;
            other.socket_fd_ =-1;
        }
        return *this;
    }

    //基础系统调用封装
    void Socket::bind(const InetAddress& addr) const{
        if (::bind(this->socket_fd_, addr.getAddr(),addr.getAddrLen()) == -1) {
            getLogger()->error("bind failed on fd={}: {}", socket_fd_, std::strerror(errno));
            throw std::system_error(errno, std::generic_category(), "bind failed");
        }
        getLogger()->info("bind success, fd={}", socket_fd_);
    }
    void Socket::listen(const int backlog) const{
        constexpr int opt = 1;

        if (::setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
            getLogger()->error("setsockopt SO_REUSEADDR failed on fd={}: {}", socket_fd_, std::strerror(errno));
            throw std::system_error(errno, std::generic_category(), "setsockopt SO_REUSEADDR failed");
        }

        #ifdef SO_REUSEPORT
        if (::setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
            getLogger()->error("setsockopt SO_REUSEPORT failed on fd={}: {}", socket_fd_, std::strerror(errno));
            throw std::system_error(errno, std::generic_category(), "setsockopt SO_REUSEPORT failed");
        }
        #endif
        if (::listen(socket_fd_, backlog) == -1) {
            getLogger()->error("listen failed on fd={}: {}", socket_fd_, std::strerror(errno));
            throw std::system_error(errno, std::generic_category(), "listen failed");
        }
        getLogger()->info("listen success, fd={}, backlog={}", socket_fd_, backlog);
    }
    std::optional<Socket> Socket::accept(InetAddress& client_addr) const {
        const int client_fd = ::accept(socket_fd_,
                               client_addr.getAddr(),
                                client_addr.getAddrLen());
        if (client_fd == -1) {
            //eagain
            if (errno  == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                return std::nullopt;
            }
            getLogger()->error("accept failed on fd={}: {}", socket_fd_, std::strerror(errno));
            throw std::system_error(errno, std::generic_category(), "accept failed");
        }

        getLogger()->info("accept success, server_fd={}, client_fd={}", socket_fd_, client_fd);
        return Socket(client_fd);
    }
    //设置非阻塞模式
    bool Socket::setNonBlocking() const{
        const auto flag{fcntl(this->socket_fd_,F_GETFL)};
        if (flag == -1) {return false;}
        return fcntl(this->socket_fd_, F_SETFL, flag | O_NONBLOCK) != -1;
    }

    ssize_t Socket::receive(void *buf, const int len, const int flags) const {
        return ::recv(this->getFd(),buf,len,flags);
    }

    ssize_t Socket::send(const void *buf, const int len,const  int flags) const {
        return ::send(this->socket_fd_,buf,len,flags);
    }
}
