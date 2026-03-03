#pragma once
#include "solitude/EventLoop.hpp"
#include "solitude/Socket.hpp"
#include "solitude/Channel.hpp"
#include "solitude/TcpConnection.hpp"

#include <cstdint>
#include <memory>
#include <map>
// #include <vector>

namespace solitude {
    class TcpServer{
        EventLoop* loop_;
        std::unique_ptr<Socket>serv_sock_;
        std::unique_ptr<Channel>serv_channel_;
        // 文件资源描述符--linux一切皆文件
        using fd = int;
        std::map<fd,std::unique_ptr<Channel>> client_channels_;
        std::map<int,std::unique_ptr<TcpConnection>>connections_;
    public:
        TcpServer(EventLoop* loop,const char* ip,uint16_t port);
        ~TcpServer();
        void handleNewConnection();
    };
}