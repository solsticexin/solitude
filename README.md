# solitude

A minimal C++ reactor-style TCP server implementation.

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## Run

```bash
./build/solitude
```

## Test Connection

```bash
nc 127.0.0.1 8888
```
