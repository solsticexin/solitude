# eirian — C++20 Reactor 网络库（Epoll + EventLoop）

`eirian` 是一个基于 Linux `epoll` 的轻量级 Reactor 网络库。项目实现了事件循环、Channel 事件分发、非阻塞 Socket 封装、缓冲区、TCP 连接与服务器等核心组件，支持回调驱动的高并发 IO 编程。

## 亮点
1. **Reactor 核心链路完整**：`Epoll -> EventLoop -> Channel -> 回调`
2. **非阻塞 IO + 输出缓冲区**：`TcpConnection::send` 自动处理短写并开启 `EPOLLOUT`
3. **可复用的 TCP 组件**：`TcpServer` 管理连接生命周期，`TcpConnection` 处理读写/关闭
4. **清晰的工程结构**：`include/` 暴露公共接口，`src/` 实现，`test/` 覆盖主要模块

## 模块结构
1. `Epoll`：对 `epoll_*` 系统调用的最小封装，提供 `add/modify/remove/poll`
2. `EventLoop`：事件循环引擎，维护 `fd -> Channel*` 映射
3. `Channel`：封装 `fd` 关注事件 + 触发回调
4. `Socket` / `InetAddress`：TCP socket 与地址封装
5. `Buffer`：字符串缓冲区，处理读写
6. `TcpConnection`：连接对象，管理读写、缓冲区、关闭回调
7. `TcpServer`：监听与连接管理（可设置消息回调）
8. `EventLoopThread`：子线程中启动 `EventLoop`
9. `EventLoopThreadPool`：线程池 + SubLoop 轮询分配

## 架构与线程模型
```
Epoll -> EventLoop -> Channel -> 业务回调
             |
             +-> TcpServer -> TcpConnection (Buffer / send / close)
```
默认单线程：`TcpServer` 使用主线程的 `EventLoop`。  
开启线程池后：新连接会被轮询分配到某个 SubLoop（`EventLoopThreadPool::getNextLoop`）。

## main.cpp 完整执行流程图
以下流程对应 `main.cpp` 中 `main()` 的执行顺序（按源码实际构造/调用链展开）：

```
main()
├── log::info("测试服务器开始")
├── try
│   ├── 构造 EventLoop loop
│   │   └── 构造 Epoll epoll_（epoll_create1 创建 epoll_fd）
│   ├── 构造 Socket server_socket
│   │   └── ::socket(AF_INET, SOCK_STREAM, 0) 创建 fd
│   ├── 构造 InetAddress addr("127.0.0.1", 8888)
│   │   ├── 填充 sockaddr_in（AF_INET / htons / inet_pton）
│   │   └── 记录 addr_len
│   ├── server_socket.bind(addr)
│   │   └── ::bind(fd, sockaddr_in)
│   ├── server_socket.listen()
│   │   ├── ::setsockopt(SO_REUSEADDR/REUSEPORT)
│   │   └── ::listen(fd)
│   ├── server_socket.setNonBlocking()
│   │   └── fcntl(F_SETFL, O_NONBLOCK)
│   ├── 构造 Channel server_channel(&loop, server_socket.getFd())
│   ├── 设置 server_channel.readCallback
│   │   └── 当 EPOLLIN 触发时：
│   │       ├── server_socket.accept(client_addr)
│   │       │   └── ::accept(fd)
│   │       ├── 构造 Socket client_socket（接管 client_fd）
│   │       ├── client_socket.setNonBlocking()
│   │       ├── 记录 client_fd / 缓冲区 / Channel
│   │       ├── 为 client_channel 设置 readCallback
│   │       │   └── ::recv -> Buffer.append -> retrieveAllAsString
│   │       │       └── ::send 回写（echo）
│   │       └── client_channel.enableReading()
│   │           └── EventLoop::updateChannel -> Epoll::add/modify
│   ├── server_channel.enableReading()
│   │   └── EventLoop::updateChannel -> Epoll::add/modify
│   ├── log::info("EventLoop 发动机启动...")
│   └── loop.loop()
│       └── while (!quit_):
│           ├── epoll_.poll(-1)
│           ├── 遍历 active fd
│           └── Channel::handleEvent -> 回调触发
└── catch (std::exception)
    └── log::error(...)
```

## 构建
要求：Linux、C++20、CMake 4.1+

```bash
cmake -S . -B build
cmake --build build -j
```

构建产物位于 `./bin`。

## 快速开始（多线程版本）
```cpp
eirian::EventLoop loop;
eirian::TcpServer server(&loop, eirian::InetAddress("127.0.0.1", 8888));
server.setThreadNum(3); // 3 个 SubLoop
server.setMessageCallback([](const std::shared_ptr<eirian::TcpConnection>& conn, std::string& msg) {
    conn->send(msg); // echo
});
server.start();
loop.loop();
```

## 运行示例
### 1. 运行内置测试服务器（手写 Channel 回调）
```bash
./bin/eirian_app
```
默认绑定 `127.0.0.1:8888`，可用 `telnet`/`nc` 进行回声测试。

### 2. TcpServer 方式（推荐）
下面是项目中可用的典型用法：

```cpp
eirian::EventLoop loop;
eirian::TcpServer server(&loop, eirian::InetAddress("127.0.0.1", 8888));
server.setMessageCallback([](const std::shared_ptr<eirian::TcpConnection>& conn, std::string& msg) {
    conn->send(msg); // echo
});
server.start();
loop.loop();
```

## 测试
项目内 `test/` 目录覆盖 `Buffer/Channel/Epoll/EventLoop/TcpConnection/...` 等组件。

```bash
ctest --test-dir build
```

目前的测试文件：
1. `testBuffer.cpp`
2. `testChannel.cpp`
3. `testEpoll.cpp`
4. `testEventLoop.cpp`
5. `testEventLoopThread.cpp`
6. `testInetAddress.cpp`
7. `testSocket.cpp`
8. `testTcpconnection.cpp`
9. `testTcpServer.cpp`

## 技术栈
1. C++20
2. Linux `epoll`
3. `spdlog` 日志库（CMake FetchContent 自动拉取）

## 设计取舍与当前状态
1. `EventLoop` 当前为单线程事件循环
2. `EventLoopThread` 已实现线程内 `loop()`，尚未提供 `quit()` 安全退出接口
3. `EventLoopThreadPool` 已支持 SubLoop 轮询分配
4. 暂未提供 Timer、任务队列/跨线程唤醒等高级功能

## 目录结构
```
.
├── include/eirian     # 头文件
├── src                # 实现
├── test               # 单元/组件测试
├── main.cpp           # 示例服务
└── CMakeLists.txt
```

## 适合简历的项目描述（可直接使用）
> 实现了基于 Linux epoll 的 C++20 Reactor 网络库，包含 EventLoop、Channel、非阻塞 Socket、缓冲区、TcpConnection/TcpServer 等核心组件；支持回调驱动的异步读写，具备输出缓冲区与 EPOLLOUT 处理，完成基础 TCP 回声服务器与多模块测试。

## TODO
1. `EventLoop::quit()` + 跨线程唤醒机制（eventfd/pipe）
2. 定时器与时间轮
3. 完整的线程池任务队列
