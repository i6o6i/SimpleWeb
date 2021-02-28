# SimpleWeb
This is a http server used Reactor pattern and ThreadPool

# BenchMark
1000 concurrent requests with 1M file test  
![1M1000](https://raw.githubusercontent.com/i6o6i/SimpleWeb/main/benchmark/1M1000.png)

1000 concurrent requests with 16M file test  
![1M1000](https://raw.githubusercontent.com/i6o6i/SimpleWeb/main/benchmark/16M1000.png)

# Build
```
cmake . && cmake --build .
```
if you wanna debug then
`cmake -DCMAKE_BUILD_TYPE=Debug . && cmake -build .`

# Thanks to

[Boost.beast](https://github.com/boostorg/beast)  
[Nginx](https://github.com/nginx/nginx)  

