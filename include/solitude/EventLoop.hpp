#pragma once 
#include <memory>
// #include <map>
// #include <vector>

namespace solitude {
    //向前声明
    class Epoll;
    class Channel;

     
    class EventLoop{
    private:
        std::unique_ptr<Epoll> epoll_ptr_;
        bool quit_{false};
    public:
        EventLoop();
        ~EventLoop();
        EventLoop(const EventLoop&)=delete;
        EventLoop& operator= (const EventLoop&)=delete;
        //启动循环
        void loop();
        //停止循环
        void quit(){quit_=true;}
        
        void updateChannel(Channel* ch);

    };
}