cmake_minimum_required(VERSION 4.2)

project(solitude VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# 开启 compile_commands.json，方便 clangd 等工具读取。
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# 当前项目未使用 C++20 Modules，关闭扫描可减少工具链要求。
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

option(SOLITUDE_BUILD_APP "Build the solitude executable." ON)
option(SOLITUDE_ENABLE_WARNINGS "Enable stricter compiler warnings." ON)

function(solitude_enable_warnings target_name)
    if (NOT SOLITUDE_ENABLE_WARNINGS)
        return()
    endif()

    if (MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /permissive-)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(${target_name} PRIVATE
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
        target_compile_options(${target_name} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wsign-conversion
            -Wshadow
            -Wformat=2
            -Wnull-dereference
            -Wdouble-promotion
            -Wtautological-compare
            -Wparentheses
            -Wold-style-cast
            -Wbool-operation
        )
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra)
    endif()
endfunction()

set(SOLITUDE_HEADERS
    include/solitude/Channel.hpp
    include/solitude/Epoll.hpp
    include/solitude/EventLoop.hpp
    include/solitude/InetAddress.hpp
    include/solitude/Socket.hpp
    include/solitude/TcpServer.hpp
    include/solitude/TcpConnection.hpp
)

set(SOLITUDE_SOURCES
    src/Channel.cpp
    src/Epoll.cpp
    src/EventLoop.cpp
    src/InetAddress.cpp
    src/Socket.cpp
    src/TcpServer.cpp
    src/TcpConnection.cpp
)

# 使用库目标承载核心逻辑，可复用、可测试，也便于安装导出。
add_library(solitude_core STATIC)
add_library(solitude::core ALIAS solitude_core)

target_sources(solitude_core
    PRIVATE
        ${SOLITUDE_SOURCES}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES ${SOLITUDE_HEADERS}
)

target_compile_features(solitude_core PUBLIC cxx_std_20)
set_target_properties(solitude_core PROPERTIES
    CXX_EXTENSIONS OFF
    OUTPUT_NAME solitude_core
)

target_include_directories(solitude_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # 使用 LLVM 生态工具，避免退回到 GNU binutils。
    find_program(LLVM_AR NAMES llvm-ar)
    find_program(LLVM_RANLIB NAMES llvm-ranlib)
    find_program(LLVM_NM NAMES llvm-nm)

    if (LLVM_AR)
        set(CMAKE_AR "${LLVM_AR}" CACHE FILEPATH "Archiver" FORCE)
    endif()
    if (LLVM_RANLIB)
        set(CMAKE_RANLIB "${LLVM_RANLIB}" CACHE FILEPATH "Ranlib" FORCE)
    endif()
    if (LLVM_NM)
        set(CMAKE_NM "${LLVM_NM}" CACHE FILEPATH "Nm" FORCE)
    endif()

    # 统一使用 libc++ + lld，形成完整 Clang 工具链。
    target_compile_options(solitude_core PUBLIC -stdlib=libc++)
    target_link_options(solitude_core PUBLIC -stdlib=libc++ -fuse-ld=lld)
endif()

solitude_enable_warnings(solitude_core)

if (SOLITUDE_BUILD_APP)
    add_executable(solitude src/main.cpp)
    target_link_libraries(solitude PRIVATE solitude::core)
    solitude_enable_warnings(solitude)
endif()

install(TARGETS solitude_core
    EXPORT solitudeTargets
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (TARGET solitude)
    install(TARGETS solitude
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

configure_package_config_file(
    cmake/solitudeConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/solitudeConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/solitude
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/solitudeConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(EXPORT solitudeTargets
    FILE solitudeTargets.cmake
    NAMESPACE solitude::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/solitude
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/solitudeConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/solitudeConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/solitude
)
