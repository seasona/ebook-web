# Jhttp_server

## 目标

- 学习一下http的相关知识，asio的使用以及项目的构建
- 能正常完成http server的基本功能，不考虑multipart和cookie
- 尝试blurhash

## 遇到的问题

### 异步写回的数据生命周期

response中必须使用全局变量，如果使用栈分配的临时变量则客户端无法收到消息，会报错`net::invalid_response_http_header`

```cpp
static const std::string response_status = "HTTP/1.1 200 OK\r\n";
buffers.push_back(boost::asio::buffer(response_status));
```

这是由于异步的特性，`boost::asio::buffer`只是作为一个标记的缓存，本身不对数据进行存储，因此当异步写回时，栈分配的临时变量已经被释放，因此会出现错误。可以使用`boost::asio::streambuf`，它是类似于流传输，保存了传输数据，因此可以确保异步写回时的数据生命周期
