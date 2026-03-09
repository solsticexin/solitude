#include "eirian/InetAddress.hpp"

#include <cstring>       // memset
#include <arpa/inet.h>
#include <string>
namespace eirian {
  InetAddress::InetAddress(const std::string& ip,const uint16_t port){
    // 1. 初始化 sockaddr_in 结构体（清空内存）
    std::memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family=AF_INET;
    //主机端转换为网络序列
    addr_.sin_port=htons(port);
    inet_pton(AF_INET,ip.c_str(),&addr_.sin_addr);
    addr_len_ = sizeof(addr_);
  }

  sockaddr* InetAddress::getAddr(){ 
    return reinterpret_cast<sockaddr*>(&addr_);
  }

  const sockaddr* InetAddress::getAddr() const{return reinterpret_cast<const sockaddr*>(&addr_);}
  socklen_t* InetAddress::getAddrLen(){
    // static auto len {static_cast<socklen_t>(sizeof(addr_))};
    // return &len;
    return &addr_len_;
  }

  socklen_t InetAddress::getAddrLen()const{
    return addr_len_;
  }

  std::string InetAddress::getIp() const{
    char buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
    return {buf};
  }

  uint16_t InetAddress::getPort() const{
    return ntohs(addr_.sin_port);
  }
}  // namespace eirian
