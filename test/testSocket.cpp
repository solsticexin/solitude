//
// Created by xin on 2026/3/6.
//
#include "eirian/InetAddress.hpp"
#include "eirian/socket.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <exception>

int main() {
    std::cout << "=== 开始测试 eirian::Socket ===\n";
    std::thread client_thread;

    try {
        // 1. 测试 Socket 创建
        const eirian::Socket server_socket{};
        std::cout << "[1] 创建 Server Socket 成功，FD: " << server_socket.getFd() << '\n';

        // 2. 测试 Bind
        const eirian::InetAddress server_addr("127.0.0.1", 8888);
        server_socket.bind(server_addr);
        std::cout << "[2] 绑定到 " << server_addr.getIp() << ":" << server_addr.getPort() << " 成功\n";

        // 3. 测试 Listen
        server_socket.listen();
        std::cout << "[3] 监听成功...\n";

        // 启动一个简单的客户端线程，模拟客户端连接请求
        client_thread = std::thread([]() {
            // 稍微等待，确保服务端已经进入 listen 状态
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            const int client_fd = ::socket(AF_INET, SOCK_STREAM, 0);
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(8888);
            inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

            if (::connect(client_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0) {
                std::cout << "    -> [模拟客户端] 连接服务器成功!\n";
            } else {
                std::cerr << "    -> [模拟客户端] 连接服务器失败!\n";
            }
            // 模拟断开连接
            ::close(client_fd);
        });

        // 4. 测试 Accept
        // 注意：InetAddress 禁用了默认构造，这里我们先用占位 IP/Port 初始化
        eirian::InetAddress client_addr("0.0.0.0", 0);

        // accept 现在返回 optional，测试中要显式判断是否接收到了连接
        auto accepted_socket = server_socket.accept(client_addr);
        if (!accepted_socket.has_value()) {
            throw std::runtime_error("accept 返回空，未接收到客户端连接");
        }
        const eirian::Socket client_socket = std::move(*accepted_socket);
        std::cout << "[4] Accept 成功! 接收到客户端连接:\n";
        std::cout << "    - 客户端 FD: " << client_socket.getFd() << '\n';
        std::cout << "    - 客户端 IP: " << client_addr.getIp() << '\n';
        std::cout << "    - 客户端 Port: " << client_addr.getPort() << '\n';

        // 5. 测试 setNonBlocking
        // setNonBlocking 带 nodiscard，必须处理返回值
        if (!server_socket.setNonBlocking()) {
            throw std::runtime_error("setNonBlocking 失败");
        }
        std::cout << "[5] 设置 Server Socket 非阻塞模式成功\n";

        // 等待客户端线程结束
        if (client_thread.joinable()) {
            client_thread.join();
        }

        std::cout << "=== 测试圆满完成 ===\n";
        return 0;
    } catch (const std::exception& e) {
        // 异常路径同样要回收线程，避免 joinable 线程析构导致终止
        if (client_thread.joinable()) {
            client_thread.join();
        }
        std::cerr << "[-] 测试失败: " << e.what() << '\n';
        return -1;
    }
}
