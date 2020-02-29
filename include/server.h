#pragma once

#include "boost/asio.hpp"
#include "connection_manager.h"
#include <string>

namespace Jhttp {

class Server {
public:
    Server(const char* address, const char* port, const char* doc_root);

    void run();

private:
    void doAccept();

    const char* doc_root_;

    boost::asio::io_context io_context_;

    boost::asio::ip::tcp::acceptor acceptor_;

    ConnectionManager connection_manager_;
};

}  // namespace Jhttp