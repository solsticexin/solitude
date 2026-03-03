#pragma once
#include "Channel.hpp"
#include "Socket.hpp"
#include <memory>
#include <string>

namespace solitude {
    class EventLoop;
    class TcpConnection:public std::enable_shared_from_this<TcpConnection>{
    private:
        EventLoop* loop_;
        std::unique_ptr<Socket> socket_;
        std::unique_ptr<Channel> channel_;
    public:
    TcpConnection(EventLoop* loop,int fd);
    ~TcpConnection();
    void send(std::string msg);
    int getFd()const{return socket_->getFd();}
    };
}