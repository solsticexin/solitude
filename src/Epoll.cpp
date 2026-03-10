//
// Created by xin on 2026/3/8.
//

#include "../include/eirian/Epoll.h"
#include "eirian/InetAddress.hpp"

#include <cstring>
#include <string>
#include <stdexcept>
#include <system_error>
#include <sys/epoll.h>
#include <unistd.h>

namespace eirian {
    Epoll::Epoll(const int max_events):epoll_events_(static_cast<size_t>(max_events)) {
        //epoll_cloexec;
        const auto fd{epoll_create1(EPOLL_CLOEXEC)};
        if (fd == -1) {
            const std::string p{"epoll_create1() failed. " + std::string(std::strerror(errno))};
            throw std::runtime_error(p);
        }
        epoll_fd_=fd;
    }
    Epoll::~Epoll() {
        if (epoll_fd_ != -1) {
            close(epoll_fd_);
            epoll_fd_ = -1;
        }
    }

    Epoll::Epoll(Epoll &&other)noexcept
        :epoll_fd_(other.epoll_fd_),epoll_events_(std::move(other.epoll_events_))  {
        other.epoll_fd_=-1;
    }

    Epoll& Epoll::operator=(Epoll &&other) noexcept {
        // 1. 防止自我赋值（移动到自己）
        if (this == &other) {
            return *this;
        }
        if (this->epoll_fd_ != -1) {
            close(this->epoll_fd_);
            this->epoll_fd_ = -1;
        }
        this->epoll_fd_=other.epoll_fd_;
        this->epoll_events_=std::move(other.epoll_events_);
        other.epoll_fd_ = -1;
        return *this;
    }

    void Epoll::add(const int fd, const uint32_t events) const {
        this->update(fd,events,EPOLL_CTL_ADD);
    }

    void Epoll::modify(const int fd, const uint32_t events) const {
        this->update(fd,events,EPOLL_CTL_MOD);
    }

    void Epoll::update(const int fd, const uint32_t events,const int op) const{
        epoll_event ev{};
        ev.events=events;
        ev.data.fd=fd;
        if (epoll_ctl(this->epoll_fd_,op,fd,&ev)==-1) {
            throw std::system_error(errno,std::generic_category(),"epoll_ctl failed!");
        }
    }

    void Epoll::remove(const int fd) const {
        if(epoll_ctl(this->epoll_fd_,EPOLL_CTL_DEL,fd,nullptr)==-1){
            throw std::system_error(errno,std::generic_category(),"epoll_ctl del failed!");
        }
    }

    std::span<epoll_event> Epoll::poll(const int timeout_ms) {
        const int num_events{epoll_wait(
            this->epoll_fd_,
            this->epoll_events_.data(),
            static_cast<int>(this->epoll_events_.size()),
            timeout_ms)};
        if (num_events==-1) {
            //被信号中断不是错误
            if (errno == EINTR){return {};}
            throw std::system_error(errno,std::generic_category(),"epoll_wait failed");
        }
        // 动态扩容逻辑（可选）
        if (static_cast<size_t>(num_events) == this->epoll_events_.size()) {
            this->epoll_events_.resize(this->epoll_events_.size() * 2);
        }
        return {this->epoll_events_.data(),static_cast<size_t>(num_events)};
    }
} // eirian
