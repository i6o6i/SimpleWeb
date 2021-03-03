# SimpleWeb
This is a http server used Reactor pattern and ThreadPool

use ThreadPool.emplace_back to add task  
[Reactor.handler_events()](https://github.com/i6o6i/SimpleWeb/blob/5721266526870f8e6276e86b1b09153172c0d28c/src/Reactor.cpp#L70)

[register demultiplexer](https://github.com/i6o6i/SimpleWeb/blob/5721266526870f8e6276e86b1b09153172c0d28c/src/SimpleWeb.cpp#L57)

[register CONNECT event handler](https://github.com/i6o6i/SimpleWeb/blob/5721266526870f8e6276e86b1b09153172c0d28c/src/SimpleWeb.cpp#L82)

[register READ And WRITE event handler](https://github.com/i6o6i/SimpleWeb/blob/5721266526870f8e6276e86b1b09153172c0d28c/src/Gateway.cpp#L141)

# BenchMark
1000 concurrent requests with 1M file test  
![1M1000](https://raw.githubusercontent.com/i6o6i/SimpleWeb/main/benchmark/1M1000.png)

1000 concurrent requests with 16M file test  
![1M1000](https://raw.githubusercontent.com/i6o6i/SimpleWeb/main/benchmark/16M1000.png)

# Requirement
`C++11`

# Build
```
cmake . && cmake --build .
```
if you wanna debug then
`cmake -DCMAKE_BUILD_TYPE=Debug . && cmake -build .`

# Thanks to

[Boost.beast](https://github.com/boostorg/beast)  
[Nginx](https://github.com/nginx/nginx)  

