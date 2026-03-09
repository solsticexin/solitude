#pragma once
#include <optional>
#include <sys/socket.h>
namespace eirian {
    class InetAddress;
    class Socket {
    private:
        int socket_fd_{-1};
    public:
    Socket();
        explicit Socket(int fd);
        ~Socket();

        Socket(const Socket&) = delete;
        Socket& operator=(const Socket&) = delete;

        Socket(Socket&&) noexcept;
        Socket& operator=(Socket&&) noexcept;

        void bind(const InetAddress& addr) const;
        void listen(int backlog=SOMAXCONN) const;
        [[nodiscard]] std::optional<Socket>  accept(InetAddress& client_addr) const;
        [[nodiscard]] bool setNonBlocking() const;

        [[nodiscard]] int getFd() const noexcept { return socket_fd_; }
    };
}
