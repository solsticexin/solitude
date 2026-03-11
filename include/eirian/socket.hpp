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

        //返回接收到的字节数， 0表示对端关闭，-1 表示错误，send一样
        [[nodiscard]] ssize_t receive(void* buf, size_t len ,int flags=0)const;
        [[nodiscard]] ssize_t send(const void* buf, size_t len ,int flags=0)const;


        [[nodiscard]] int getFd() const noexcept { return socket_fd_; }
    };
}
