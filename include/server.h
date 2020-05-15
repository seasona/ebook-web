#pragma once

#include "asio.hpp"
#include "connection_manager.h"
#include <string>

namespace Jepub {

class Server {
public:
    Server(const char* address, const char* port, const char* doc_root);

    void setTemplatePath(std::string template_path);

    void run();

private:
    void doAccept();

    const char* doc_root_;

    asio::io_context io_context_;

    asio::ip::tcp::acceptor acceptor_;

    ConnectionManager connection_manager_;

    Templates templates_;
};

}  // namespace Jepub