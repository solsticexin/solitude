#pragma once
#include <sys/epoll.h>
#include <vector>

namespace solitude {
    class Channel;
    class Epoll{
    private:
        int epoll_fd_;
        std::vector<epoll_event> events_;
    public:
        Epoll();
        ~Epoll();
        //把channel注册到"监控室"
        void updateChannel(Channel* ch);
        std::vector<Channel*> poll(int timeout_ms =-1);
    };
}