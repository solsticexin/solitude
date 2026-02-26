cmake_minimum_required(VERSION 4.2)

project(solitude LANGUAGES CXX)

# # Avoid requiring clang-scan-deps when not using C++20 modules.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(solitude
    src/Channel.cpp
    src/Epoll.cpp
    src/EventLoop.cpp
    src/InetAddress.cpp
    src/Socket.cpp
    src/TcpServer.cpp
    src/main.cpp
)

target_include_directories(solitude
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# High warnings suitable for beginners: strong defaults without pedantic or Werror.
if (MSVC)
    target_compile_options(solitude PRIVATE /W4 /permissive-)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(solitude PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wshadow
        -Wformat=2
        -Wnull-dereference
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op
        -Wuseless-cast
        -Wdouble-promotion
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(solitude PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wshadow
        -Wformat=2
        -Wnull-dereference
        -Wdouble-promotion
        # Clang 等价或类似警告（可选添加，覆盖部分 GCC 专有功能）
        -Wtautological-compare          # 部分覆盖 duplicated-cond
        -Wparentheses                   # 帮助逻辑运算符误用
        -Wold-style-cast                # 类似 useless-cast 的旧式转换
        -Wbool-operation                # 逻辑运算相关
    )
else()
    # 其他编译器（如未知）用基本警告
    target_compile_options(solitude PRIVATE -Wall -Wextra)
endif()
