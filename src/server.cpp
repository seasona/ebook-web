#include "server.h"
#include "spdlog/spdlog.h"

namespace Jhttp {

Server::Server(const char* address, const char* port, const char* doc_root)
    : doc_root_(doc_root),
      io_context_(1),
      acceptor_(io_context_),
      connection_manager_() {
    boost::asio::ip::tcp::resolver resolver(io_context_);
    boost::asio::ip::tcp::endpoint endpoint =
        *(resolver.resolve(address, port).begin());

    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    doAccept();
}

void Server::doAccept() {
    acceptor_.async_accept([this](boost::system::error_code ec,
                                  boost::asio::ip::tcp::socket socket) {
        // TODO check
        if (!acceptor_.is_open()) {
            spdlog::error("The acceptor is not open.");
            return;
        }

        if (!ec) {
            spdlog::debug("Accepted a socket.");
            connection_manager_.start(std::make_shared<Connection>(
                std::move(socket), connection_manager_, doc_root_));
        }

        doAccept();
    });
}

void Server::run() {
    spdlog::info("The server is running.");
    io_context_.run();
}

}  // namespace Jhttp