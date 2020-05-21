#pragma once

#include <string>
#include <memory>
#include <array>
#include "asio.hpp"
#include "request.h"
#include "response.h"
#include "llhttp.h"
#include "templates.h"

namespace Jebook {

class ConnectionManager;

class Connection : public std::enable_shared_from_this<Connection> {
public:
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    Connection(asio::ip::tcp::socket socket,
               ConnectionManager& connection_manager, const char* doc_root,
               Templates& t);

    ~Connection();

    void start();

    void stop();

private:
    // connection read from socket, get the resquest buffer
    void doRead();

    // connection write the response into the socket, use boost::asio::buffer
    // because buffer don't store data, just a reference and size container, so
    // the data in buffer must exist before async operation.
    void doWrite();

    // boost::asio::streambuf dislike boost::asio::buffer, it like stream and
    // store the input data, so can use async operation casually
    void doWriteByStreamBuf();

    // handle the request data to produce response
    int handleRequest(const Request& request, Response& response);

    // find the Content-Type according to the file type
    std::string extensionToType(const std::string& extension);

    // url decode 
    std::string urlDecode(const std::string &str_source);

    void initParser();

    void initParserSetting();

    // callback of llhttp parser
    static int onUrl(llhttp_t* parser, const char* at, size_t length);
    static int onBody(llhttp_t* parser, const char* at, size_t length);

    static int onHeaderField(llhttp_t* parser, char const* at, size_t length);
    static int onHeaderValue(llhttp_t* parser, char const* at, size_t length);
    // static int onHeadersComplete(llhttp_t* parser);

    // static int onMessageBegin(llhttp_t* parser);
    // static int onMessageComplete(llhttp_t* parser);

    // reset the data in request and response every time
    void reset();

    asio::ip::tcp::socket socket_;

    // buffer for incoming read data
    std::array<char, 8192> buffer_;

    llhttp_settings_t parser_setting_;

    std::unique_ptr<llhttp_t> parser_;

    Request request_;

    Response response_;

    asio::streambuf write_buf_;

    ConnectionManager& connection_manager_;

    const char* doc_root_;

    Templates& templates_;
};

typedef std::shared_ptr<Connection> ConnectionPtr;

}  // namespace Jebook