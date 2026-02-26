#include "solitude/Channel.hpp"
#include "solitude/EventLoop.hpp"
#include <sys/epoll.h>

namespace solitude {
    Channel::Channel(EventLoop* loop,int fd):fd_(fd),events_(0),revents_(0),loop_(loop){}
    void Channel::handleEvent(){
        //如果实际发生的事件包含“读”或者“挂断/错误”
        if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP) ) {
            if (read_callback_) {
                read_callback_();
            }
        }
    }

    void Channel::enableReading(){
        events_ |= EPOLLIN; //加上读事件
        loop_ ->updateChannel(this);
    }
}