#include "solitude/TcpServer.hpp"
#include "solitude/EventLoop.hpp"
#include "solitude/InetAddress.hpp"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <format>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace solitude {
    TcpServer::TcpServer(EventLoop* loop,const char* ip,uint16_t port)
    :loop_(loop),
    serv_sock_(std::make_unique<Socket>()){
        // InetAddress addr(ip,port);
        InetAddress addr{ip,port};
        serv_sock_ ->bind(addr);
        serv_sock_ ->listen();
        serv_sock_ ->setNonBlocking();

        //创建保姆
        serv_channel_ = std::make_unique<Channel>(loop_,serv_sock_->getFd());
        
        //核心
        serv_channel_ ->setReadCallBack([this](){
            this->handleNewConnection();
        });
        serv_channel_ -> enableReading();
    }

    TcpServer::~TcpServer()=default;
    void TcpServer::handleNewConnection(){
        InetAddress client_addr{};
        int client_fd {serv_sock_ ->accept(client_addr)};
        if (client_fd != -1) {
            std::cout << std::format("TcpServer 监听到新连接:{},{}\n",client_addr.getIp(),client_addr.getPort());
            auto client_ch {std::make_unique<Channel>(loop_,client_fd)};
            client_ch->setReadCallBack([client_fd](){
                char buf[1024];
                ssize_t n = read(client_fd, buf, sizeof(buf));
                if ( n>0) {
                    std::cout << std::format("收到fd{}的数据，准备回传\n",client_fd);
                    send(client_fd, buf, static_cast<size_t>(n), 0); //原样发回
                }else if(n == 0){
                    std::cout << std::format("clinet fd: {} 断开连接\n",client_fd);
                    close(client_fd);
                }
            });

            client_ch->enableReading();
            client_channels_[client_fd] = std::move(client_ch);
        }
    }
}