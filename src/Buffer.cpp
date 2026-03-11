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

    // 仅仅把前面的 len 个字节从水缸里删掉（表示这部分已经发给内核了）
    void Buffer::retrieve(const size_t len) {
        if (len >= buffer_.size()) {
            buffer_.clear();
        } else {
            buffer_.erase(0, len); // 擦除前 len 个字节
        }
    }

    std::string Buffer::retrieveAllAsString() {
        std::string result {std::move(this->buffer_)};
        this->buffer_.clear();
        return result;
    }
}
