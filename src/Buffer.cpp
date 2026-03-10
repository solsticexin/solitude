//
// Created by xin on 2026/3/10.
//

#include "../include/eirian/Buffer.h"

namespace eirian {
    void Buffer::append(const char *data,const  int len) {
        this->buffer_ .append(data,len);
    }

    void Buffer::append(const std::string &data) {
        this->buffer_ .append(data);
    }

    size_t Buffer::readableBytes() const {
        return this->buffer_.size();
    }

    void Buffer::retrieve(const size_t len) {
        if (len > this->buffer_.size()) {
            this->buffer_.clear();
        }else {
            this->buffer_.erase(0,len);
        }
    }

    std::string Buffer::retrieveAllAsString() {
        std::string result {std::move(this->buffer_)};
        this->buffer_.clear();
        return result;
    }
}
