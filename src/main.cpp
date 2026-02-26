#include "solitude/EventLoop.hpp"
#include "solitude/TcpServer.hpp"
#include <iostream>

int main(){
    solitude::EventLoop loop{};
    solitude::TcpServer server{&loop,"127.0.0.1",8888};
    std::cout << "Reactor 服务器启动成功,等待连接...\n";
    loop.loop();
}