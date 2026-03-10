//
// Created by xin on 2026/3/8.
//

#ifndef EIRIAN_EPOLL_H
#define EIRIAN_EPOLL_H
#include <span>
#include <vector>
#include <sys/epoll.h>

namespace eirian {
    class Epoll {
        int epoll_fd_{};
        //设置预设，与分配的事件
        std::vector<epoll_event> epoll_events_;
        void update(int fd,uint32_t events,int op=EPOLL_CTL_ADD) const;
    public:
        // explicit Epoll(const int fd) : epoll_fd_(fd) {};
        explicit Epoll(int max_events=1024);
        ~Epoll();
        Epoll(const Epoll&) = delete;
        Epoll& operator=(const Epoll&) = delete;

        Epoll(Epoll&& other) noexcept;
        Epoll& operator=(Epoll&& other) noexcept;

        void add(int fd,uint32_t events)const;
        void modify(int fd,uint32_t events)const;
        void remove(int fd) const;

        //等待事件,返回已经激活的事件
        std::span<epoll_event> poll(int timeout_ms =-1);

    };
} // eirian

#endif //EIRIAN_EPOLL_H