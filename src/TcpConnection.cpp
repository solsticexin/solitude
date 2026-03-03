#include "solitude/TcpConnection.hpp"
#include "solitude/Channel.hpp"
#include "solitude/EventLoop.hpp"
#include "solitude/Socket.hpp"

#include <cstddef>
#include <memory>
#include <iostream>
#include <format>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
namespace solitude {
    TcpConnection::TcpConnection(EventLoop* loop,int fd):loop_(loop),socket_(std::make_unique<Socket>(fd)){
        channel_ = std::make_unique<Channel>(loop_,socket_->getFd());
        channel_ -> setReadCallBack([this](){
            char buf[1024]{};
            auto n{read(socket_->getFd(), buf, sizeof(buf))};
            if (n >0) {
                std::cout << std::format("收到fd:{}的数据,长度{}\n",socket_->getFd(),n);
                send(std::string(buf,static_cast<size_t>(n)));
            }else if (n==0) {
                std::cout << std::format("客户端fd:{}断开连接\n",socket_->getFd());
            }
        });
        channel_->enableReading();
    };
    TcpConnection::~TcpConnection()=default;
    void TcpConnection::send(std::string msg){
        ::send(socket_->getFd(), msg.c_str(), msg.size(), 0);
    }
}