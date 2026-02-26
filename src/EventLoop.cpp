#include "solitude/EventLoop.hpp"
#include "solitude/Epoll.hpp"
#include "solitude/Channel.hpp"
#include <iostream>
#include <memory>
#include <vector>

namespace solitude {
    EventLoop::EventLoop():epoller_(std::make_unique<Epoll>()){};
    EventLoop::~EventLoop()=default;
    void EventLoop::loop(){
        std::cout << "Eventloop 开始启动，进入事件循环...\n";
        while (!quit_) {
            std::vector<Channel*> active_channels{epoller_->poll(-1)}; //-1表示无限等待
            for (auto* ch : active_channels) {
                ch->handleEvent();
            }
        }
    }
    void EventLoop::updateChannel(Channel* ch){
        epoller_->updateChannel(ch);
    }
}