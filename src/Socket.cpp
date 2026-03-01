#include "solitude/Socket.hpp"

#include <asm-generic/socket.h>
#include <cstdlib>
#include <format>
#include <iostream>
#include <netinet/in.h>
#include <string_view>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

namespace solitude {
    namespace{
        // 用途：统一输出致命错误信息并终止进程，避免重复错误处理代码。
        // 参数 msg：错误场景说明字符串（例如 "bind failed"）。
        void report_error(std::string_view msg){
            // 变量 errc：把当前 errno 封装成标准错误码，便于获取错误文本和数值。
            const std::error_code errc(errno,std::generic_category());
            std::cerr << std::format("Fatal Error:{}-{} ({})\n",msg,errc.message(),errc.value());
            exit(EXIT_FAILURE);
        }
    }

    // 用途：创建一个 IPv4 TCP 套接字，并保存其文件描述符。
    Socket::Socket(){
        // socket(domain, type, protocol)
        // 参数1 domain: 协议族，AF_INET 表示 IPv4。
        // 参数2 type: 套接字类型，SOCK_STREAM 表示面向连接的 TCP 字节流。
        // 参数3 protocol: 具体协议，0 表示让内核按 domain+type 自动选择（此处为 TCP）。
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ == -1) {
            report_error("socket creation failed");
        }
    }

    // 用途：接管一个已经存在的套接字文件描述符。
    // 参数 fd：外部已创建好的套接字文件描述符。
    Socket::Socket(int fd):fd_(fd){}

    // 用途：析构时关闭套接字，确保系统资源被正确释放。
    Socket::~Socket(){
        if (fd_ != -1) {
            // close(fd)
            // 参数 fd: 需要关闭的文件描述符；关闭后内核释放该套接字相关资源。
            close(fd_);
            fd_ = -1;
        }
    }

    // 用途：通过移动构造转移套接字所有权，避免拷贝文件描述符。
    // 参数 other：被转移资源的源对象。
    Socket::Socket(Socket&& other) noexcept:fd_(other.fd_){}

    // 用途：通过移动赋值转移套接字所有权，并处理旧资源释放。
    // 参数 other：被转移资源的源对象。
    Socket& Socket::operator= (Socket&& other)noexcept{
        if (this != &other) {
            if (fd_ != -1) {
                // close(fd)
                // 用途：在接管新 fd 前先关闭当前对象持有的旧 fd，避免文件描述符泄漏。
                // 参数 fd: 当前对象已持有的套接字文件描述符。
                close(fd_);
            }
            fd_ =other.fd_;
            other.fd_ = -1;
        }
        return *this;

    }

    //基础系统调用封装
    // 用途：为套接字设置地址复用并绑定本地 IP/端口。
    // 参数 addr：要绑定的本地地址信息（IP + 端口）。
    void Socket::bind(const InetAddress& addr){
        // 变量 opt：setsockopt 的开关值，1 表示启用 SO_REUSEADDR。
        int opt{1};
        // setsockopt(sockfd, level, optname, optval, optlen)
        // 用途：设置套接字选项，这里开启地址复用，避免端口短时间内无法重新绑定。
        // 参数1 sockfd: 目标套接字文件描述符。
        // 参数2 level: 选项层级，SOL_SOCKET 表示套接字通用层。
        // 参数3 optname: 选项名，SO_REUSEADDR 允许地址快速复用。
        // 参数4 optval: 选项值地址；此处 &opt（1 表示开启）。
        // 参数5 optlen: 选项值长度；此处 sizeof(opt)。
        setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        // bind(sockfd, addr, addrlen)
        // 用途：把套接字与本地地址（IP/端口）绑定，后续 listen/accept 才能在该地址上工作。
        // 参数1 sockfd: 要绑定的套接字文件描述符。
        // 参数2 addr: 本地地址结构体指针（IP + 端口），此处来自 InetAddress。
        // 参数3 addrlen: 地址结构体长度；IPv4 使用 sizeof(sockaddr_in)。
        if (::bind(fd_, addr.getAddr(), sizeof(sockaddr_in)) == -1) {
            report_error("bind failed");
        }
    }
    // 用途：把套接字切换为监听状态，开始接收连接请求。
    void Socket::listen(){
        // listen(sockfd, backlog)
        // 用途：将已绑定的套接字标记为监听套接字，开始接收来自客户端的连接请求。
        // 参数1 sockfd: 已完成 bind 的监听套接字文件描述符。
        // 参数2 backlog: 全连接队列上限；SOMAXCONN 让内核使用系统允许的最大值。
        if (::listen(fd_, SOMAXCONN) == -1) {
            report_error("listen failed");
        }
    }
    // 用途：从监听套接字中接受一个新连接，并回填客户端地址。
    // 参数 client_addr：输出参数，保存新连接客户端的地址信息。
    // 返回值：成功返回新连接 fd，失败返回 -1。
    int Socket::accept(InetAddress& client_addr){
        // accept(sockfd, addr, addrlen)
        // 用途：从监听队列中取出一个已建立连接，返回新的通信套接字 fd。
        // 参数1 sockfd: 监听套接字文件描述符。
        // 参数2 addr: 输出参数，成功后写入客户端地址（IP + 端口）。
        // 参数3 addrlen: 输入输出参数，传入缓冲区大小，返回实际写入长度。
        // 变量 client_fd：accept 返回的新连接文件描述符。
        auto client_fd {::accept(fd_, client_addr.getAddr(), client_addr.getAddrLen())};
        if (client_fd == -1){
            std::cerr << "accept failed(EAGAIN typically in non-blocking) \n";
        }
        return client_fd;
    }

    //设置非阻塞模式
    // 用途：将当前套接字设置为非阻塞模式，避免阻塞线程。
    void Socket::setNonBlocking(){
        // fcntl(fd, cmd, ...)
        // 用途：读取当前 fd 的文件状态标志，作为后续设置非阻塞的基础。
        // 参数1 fd: 目标文件描述符。
        // 参数2 cmd: F_GETFL，读取当前文件状态标志（如 O_NONBLOCK）。
        // 参数3 ...: 对 F_GETFL 通常忽略；此处传 0。
        // 变量 flags：当前文件状态标志位（在其基础上追加 O_NONBLOCK）。
        auto flags{fcntl(fd_, F_GETFL,0)};
        if (flags == -1) {
            report_error("fcntl F_GETFL failed");
        }

        // fcntl(fd, cmd, ...)
        // 用途：写回新的文件状态标志，使该套接字进入非阻塞模式。
        // 参数1 fd: 目标文件描述符。
        // 参数2 cmd: F_SETFL，设置文件状态标志。
        // 参数3 ...: 新标志值；在原有 flags 基础上按位或 O_NONBLOCK 开启非阻塞。
        if (fcntl(fd_, F_SETFL,flags | O_NONBLOCK) == -1) {
            report_error("fcntl F_SETFL failed");
        }
    }
        
}
