# solitude

基于 Reactor 模型的最小 C++ TCP 服务器示例，项目已按现代 CMake 方式组织：

- `solitude_core`：核心静态库目标（可复用、可安装）
- `solitude`：示例可执行程序（链接 `solitude_core`）
- `CMakePresets.json`：预设构建配置（含 Clang + libc++）
- `.vscode/tasks.json`：VSCode 一键编译任务

## 一键编译（推荐）

```bash
cmake --preset clang-debug
cmake --build --preset clang-debug
```

## 运行

```bash
./build/clang-debug/solitude
```

## 连接测试

```bash
nc 127.0.0.1 8888
```

## VSCode 一键编译

打开命令面板后执行 `Tasks: Run Build Task`，默认任务为：

- `一键编译(Clang Debug)`
