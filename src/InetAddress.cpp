#include "solitude/InetAddress.hpp"
#include <cstdint>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace solitude{
    // 用途：根据给定 IPv4 字符串和端口构造套接字地址结构。
    // 参数 ip：点分十进制 IPv4 地址字符串（如 "127.0.0.1"）。
    // 参数 port：主机字节序端口号（如 8080）。
    InetAddress::InetAddress(const char* ip,uint16_t port){
        //ipv4
        // 变量 addr_：内部持有的 sockaddr_in，保存协议族、端口、IP。
        addr_.sin_family=AF_INET;
        // htons(hostshort)
        // 用途：把主机字节序端口转换为网络字节序，保证跨机器网络传输一致。
        // 参数 hostshort: 主机字节序 16 位端口号。
        addr_.sin_port=htons(port);

        // inet_pton(AF_INET, src, dst)
        // 用途：把文本 IP（如 "127.0.0.1"）转换为二进制网络地址，写入 sockaddr_in。
        // 参数1 AF_INET：将文本 IPv4 地址转换为网络字节序二进制地址。
        // 参数2 ip：输入的文本 IP 字符串。
        // 参数3 &addr_.sin_addr：输出目标，写入转换后的 IPv4 地址。
        inet_pton(AF_INET,ip,&addr_.sin_addr);
    }

    // 用途：返回可写的通用 sockaddr 指针，供 bind/accept 等系统调用使用。
    sockaddr* InetAddress::getAddr(){
        return reinterpret_cast<sockaddr*> (&addr_);
    }

    // 用途：返回只读的通用 sockaddr 指针，供不修改地址的调用场景使用。
    const sockaddr* InetAddress::getAddr() const{
        return reinterpret_cast<const sockaddr*>(&addr_);
    }
    
    // 用途：返回地址结构长度指针，供 accept 等需要 socklen_t* 的接口使用。
    socklen_t* InetAddress::getAddrLen(){
        // 变量 len：地址结构长度（sockaddr_in），静态存储期避免返回悬空地址。
        static socklen_t len { sizeof(addr_)};
        return &len;
    }

    //辅助函数
    // 用途：把内部二进制 IPv4 地址转换为可读字符串并返回。
    std::string InetAddress::getIp() const{
        // 变量 ip_buf：用于接收 inet_ntop 输出的 IPv4 文本缓冲区。
        char ip_buf[INET_ADDRSTRLEN]{};
        // inet_ntop(AF_INET, src, dst, size)
        // 用途：把二进制网络地址转换为人类可读的点分十进制字符串。
        // 参数1 AF_INET：按 IPv4 方式进行地址到文本转换。
        // 参数2 &addr_.sin_addr：输入的二进制 IPv4 地址。
        // 参数3 ip_buf：输出缓冲区，写入点分十进制字符串。
        // 参数4 sizeof(ip_buf)：输出缓冲区大小，防止越界。
        inet_ntop(AF_INET,&addr_.sin_addr,ip_buf,sizeof(ip_buf));
        return std::string(ip_buf);
    }

    // 用途：获取主机字节序端口号，便于业务代码直接使用。
    uint16_t InetAddress::getPort() const {
        //网络序转主机序
        // ntohs(netshort)
        // 用途：把网络字节序端口还原为主机字节序，便于打印和业务处理。
        // 参数 netshort: 网络字节序 16 位端口号。
        return ntohs(addr_.sin_port);
    }
}
