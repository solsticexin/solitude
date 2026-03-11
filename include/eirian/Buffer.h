//
// Created by xin on 2026/3/10.
//

#ifndef EIRIAN_BUFFER_H
#define EIRIAN_BUFFER_H

#include <string>
#include <vector>

namespace eirian {
    class Buffer {
    private:
        std::string buffer_;
    public:
        Buffer()=default;
        ~Buffer()=default;

        void append(const char* data,int len);
        void append(const std::string& data);

        [[nodiscard]] size_t readableBytes()const;

        void retrieve(size_t len);

        [[nodiscard]]std::string retrieveAllAsString();

        [[nodiscard]] const char* peek() const {
            return buffer_.data();
        }
    };
}


#endif //EIRIAN_BUFFER_H