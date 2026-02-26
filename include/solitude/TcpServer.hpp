#pragma once
#include "solitude/EventLoop.hpp"
#include "solitude/Socket.hpp"
#include "solitude/Channel.hpp"

#include <cstdint>
#include <memory>
// #include <vector>

namespace solitude {
    class TcpServer{
        EventLoop* loop_;
        std::unique_ptr<Socket>serv_sock_;
        std::unique_ptr<Channel>serv_channel_;
    public:
        TcpServer(EventLoop* loop,const char* ip,uint16_t port);
        ~TcpServer();
        void handleNewConnection();
    };
}