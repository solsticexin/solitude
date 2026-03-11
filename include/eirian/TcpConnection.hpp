//
// Created by xin on 2026/3/11.
//

#ifndef EIRIAN_TCPCONNECTION_HPP
#define EIRIAN_TCPCONNECTION_HPP

#include "eirian/Buffer.h"
#include "eirian/socket.hpp"
#include "eirian/Channel.hpp"

#include <memory>
#include <string>
#include <functional>

namespace eirian {
    class TcpConnection:public std::enable_shared_from_this<TcpConnection>{
    private:
        EventLoop* loop_;
        std::unique_ptr<Socket> socket_;
        std::unique_ptr<Channel> channel_;
        Buffer inputBuffer_;
        Buffer outputBuffer_;

        std::function<void (std::shared_ptr<TcpConnection>,std::string&)> messageCallback_;
        std::function<void (std::shared_ptr<TcpConnection>)> closeCallback_;

        // Channel 触发的底层处理函数
        void handleRead();
        void handleWrite(); // 预留
        void handleClose();
        void handleError();
    public:
        // 传入大管家 loop 和 已经 accept 出来的客户端 Socket
        TcpConnection(EventLoop* loop, Socket&& socket);
        ~TcpConnection();

        void setMessageCallback(std::function<void (std::shared_ptr<TcpConnection>,std::string&)> cb ) {
            messageCallback_=std::move(cb);
        }
        void setCloseCallback(std::function<void(std::shared_ptr<TcpConnection>)> cb) {
            closeCallback_=std::move(cb);
        }

        void send(const std::string& msg);

        // 连接建立后，由 Server 统一调用，启动事件监听
        void connectEstablished() const;

        // 连接销毁前调用，清理事件
        void connectDestroyed() const;

        // 获取底层 fd，方便打日志
        int getFd() const { return socket_->getFd(); }
    };
} // eirian

#endif //EIRIAN_TCPCONNECTION_HPP
