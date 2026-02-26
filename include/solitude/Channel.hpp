#pragma once
#include "solitude/EventLoop.hpp"
#include <functional>
#include <cstdint>
#include <utility>

namespace solitude {
    class Channel{
        int fd_;
        uint32_t events_;   //希望监听事件
        uint32_t revents_;  //实际发生事件有epoll填入
        std::function<void()> read_callback_;    
        EventLoop* loop_;
    public:
        Channel(EventLoop* loop,int fd);
        ~Channel()=default;

        //有epoll调用，告诉channle：你的fd文件活跃了
        void handleEvent();

        //设置回调函数
        void setReadCallBack(std::function<void()> cb){read_callback_ = std::move(cb);}

        int getFd() const{return fd_;}
        uint32_t getEvents() const{return events_;}
        void setRevents(uint32_t ev){revents_ = ev;}
        void enableReading(); //开启读监听
    };
}