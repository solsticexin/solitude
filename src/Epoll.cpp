#include "solitude/Epoll.hpp"
#include "solitude/Channel.hpp"
#include <cstddef>
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>

//学习用的语法

solitude::Epoll::Epoll():events_(64){
    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ == -1) {
        std::cerr << "Epoll 创建失败";
    }
}

namespace solitude {
    Epoll::~Epoll(){
        if (epoll_fd_ == -1) {
            close(epoll_fd_);
        }
    }

    void Epoll::updateChannel(Channel* ch){
        epoll_event ev{};
        ev.events = ch->getEvents();
        ev.data.ptr =ch;

        //简化先默认添加操作
        epoll_ctl(epoll_fd_, EPOLL_CTL_ADD,ch->getFd(), &ev);
    }
    std::vector<Channel*> Epoll::poll(int timeout_ms){
        int nfds {epoll_wait(epoll_fd_, 
                             events_.data(), 
                             static_cast<int>(events_.size()), 
                             timeout_ms)};
        std::vector<Channel*> active_channels;
        for (int i{0}; i<nfds; ++i) {
            Channel* ch {static_cast<Channel*>(events_[static_cast<size_t>(i)].data.ptr)};
            ch->setRevents(events_[static_cast<size_t>(i)].events);
            active_channels.push_back(ch);
        }
        return  active_channels;
    }
}
