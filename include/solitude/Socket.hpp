#pragma once

#include "InetAddress.hpp"

namespace solitude {
    class Socket{
    private:
        int fd_{-1};
    public:
        Socket();
        explicit Socket(int fd);
        ~Socket();
        //禁止拷贝
        Socket(const Socket&)=delete;
        Socket& operator=(const Socket&)=delete;

        //允许移动构造和移动赋值
        Socket(Socket&& other) noexcept;
        Socket& operator = (Socket&& other)noexcept;

        //基础系统调用封装
        void bind(const InetAddress& addr);
        void listen();
        int accept(InetAddress& client_addr);

        //设置非阻塞模式
        void setNonBlocking();
        
        int getFd()const{return fd_;}

    };
}