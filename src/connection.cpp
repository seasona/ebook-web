#include "connection.h"

#include <fstream>
#include <unordered_map>

#include "connection_manager.h"
#include "spdlog/spdlog.h"
#include "util.h"

namespace Jebook {

static std::unordered_map<std::string, std::string> types = {
    {"gif", "image/gif"},  {"htm", "text/html"}, {"html", "text/html"},
    {"jpg", "image/jpeg"}, {"png", "image/png"}, {"css", "text/css"}};

static inline int htoi(char *s) {
    int value;
    int c;

    c = ((unsigned char *)s)[0];
    if (isupper(c)) c = tolower(c);
    value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

    c = ((unsigned char *)s)[1];
    if (isupper(c)) c = tolower(c);
    value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

    return (value);
}

std::string Connection::urlDecode(const std::string &str_source) {
    char const *in_str = str_source.c_str();
    int in_str_len = strlen(in_str);
    std::string out_str;
    char *str;

    str = strdup(in_str);

    char *dest = str;
    char *data = str;

    while (in_str_len--) {
        if (*data == '+') {
            *dest = ' ';
        } else if (*data == '%' && in_str_len >= 2 &&
                   isxdigit((int)*(data + 1)) && isxdigit((int)*(data + 2))) {
            *dest = (char)htoi(data + 1);
            data += 2;
            in_str_len -= 2;
        } else {
            *dest = *data;
        }
        data++;
        dest++;
    }
    *dest = '\0';
    out_str = str;
    free(str);
    return out_str;
}

Connection::Connection(asio::ip::tcp::socket socket,
                       ConnectionManager& connection_manager,
                       const char* doc_root, Templates& t)
    : socket_(std::move(socket)),
      connection_manager_(connection_manager),
      doc_root_(doc_root),
      templates_(t),
      parser_(new llhttp_t) {
    initParserSetting();
    spdlog::debug("Parser setting init done.");
    initParser();
    spdlog::debug("Http parser init done.");
}

Connection::~Connection() {}

void Connection::start() {
    spdlog::debug("Started a connection.");
    doRead();
}

void Connection::stop() { socket_.close(); }

void Connection::doRead() {
    // This is done in order to make sure that connection object outlives the
    // asynchronous operation: as long as the lambda is alive (i.e. the async.
    // operation is in progress), the connection instance is alive as well.
    // https://stackoverflow.com/questions/37036611/asio-usage-of-self-shared-pointer-in-the-examples
    auto self(shared_from_this());
    socket_.async_read_some(
        asio::buffer(buffer_),
        [this, self](asio::error_code ec, std::size_t bytes_transferred) {
            if (ec) {
                if (ec != asio::error::eof) {
                    spdlog::error("Client connection closed not graceful. {}",
                                  ec.message());
                }
                // TODO just return or stop in connection_manager?
                connection_manager_.stop(shared_from_this());
            } else {
                spdlog::trace("Http request:\n{}", buffer_.data(),
                              bytes_transferred);
                auto parsed_errno = llhttp_execute(
                    parser_.get(), buffer_.data(), bytes_transferred);

                spdlog::debug("Http parse done. return {0:d}", parsed_errno);

                if (parsed_errno != HPE_OK) {
                    spdlog::error("Http request parse error.");
                    return;
                }

                request_.version_major = parser_->http_major;
                request_.version_minor = parser_->http_minor;
                int err = handleRequest(request_, response_);
                if (err != 0) {
                    spdlog::error("Handle request failed.");
                }
                // doWrite();
                doWriteByStreamBuf();
            }
        });
}

void Connection::initParser() {
    llhttp_init(parser_.get(), HTTP_REQUEST, &parser_setting_);
    // pass the pointer of connection to the hook in parser
    parser_->data = this;
}

void Connection::initParserSetting() {
    llhttp_settings_init(&parser_setting_);

    parser_setting_.on_url = &Connection::onUrl;
    parser_setting_.on_body = &Connection::onBody;
    parser_setting_.on_header_field = &Connection::onHeaderField;
    parser_setting_.on_header_value = &Connection::onHeaderValue;
}

int Connection::onUrl(llhttp_t* parser, const char* at, size_t length) {
    Connection* cnt = static_cast<Connection*>(parser->data);
    cnt->request_.uri.append(at, length);
    spdlog::debug("Url is {}", cnt->request_.uri);
    return HPE_OK;
}

int Connection::onBody(llhttp_t* parser, const char* at, size_t length) {
    Connection* cnt = static_cast<Connection*>(parser->data);
    // TODO need to add the situation when data overpass max
    cnt->request_.body.append(at, length);

    return HPE_OK;
}

// TODO the field and value may be unordered or overlaped, need test
int Connection::onHeaderField(llhttp_t* parser, const char* at, size_t length) {
    Connection* cnt = static_cast<Connection*>(parser->data);
    cnt->request_.header_field.push_back(std::string(at, length));

    return HPE_OK;
}

int Connection::onHeaderValue(llhttp_t* parser, char const* at, size_t length) {
    Connection* cnt = static_cast<Connection*>(parser->data);
    cnt->request_.header_value.push_back(std::string(at, length));

    return HPE_OK;
}

void Connection::reset() {
    request_.uri.clear();
    request_.header_field.clear();
    request_.header_value.clear();
    request_.body.clear();

    response_.content.clear();
    response_.headers.clear();
}

std::string Connection::extensionToType(const std::string& extension) {
    auto it = types.find(extension);
    return (it != types.end()) ? it->second : "text/plain";
}

int Connection::handleRequest(const Request& request, Response& response) {
    std::string request_path = urlDecode(request.uri);
    response.version_major = request.version_major;
    response.version_minor = request.version_minor;

    if (request_path.empty() || request_path[0] != '/' ||
        request_path.find("..") != std::string::npos) {
        spdlog::error("The request_path has mistake. request_path = {}",
                      request_path);
        return -1;
    }

    // TODO reduce the degree of coupling
    // read the template.html and fullfill it with toc.ncx
    if (request_path[request_path.size() - 1] == '/') {
        // request_path += templates_.getTemplatePath();
        response_.content.append(templates_.parse());

        // spdlog::debug("Content:\n{}", response.content);
        spdlog::debug("Content-Length: {0:d}", response.content.size());
        // template auto deduction
        response.headers.push_back(std::make_pair(
            "Content-Length", std::to_string(response.content.size())));
        response.headers.push_back(
            std::make_pair("Content-Type", extensionToType("html")));

        return 0;
    }

    // deduce the Content-Type by the file type
    std::size_t last_slash_pos = request_path.find_last_of("/");
    std::size_t last_dot_pos = request_path.find_last_of(".");
    std::string extension;
    if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
        extension = request_path.substr(last_dot_pos + 1);
    }

    // TODO: need to decode url
    std::string full_path = Util::normalize(doc_root_ + request_path);
    
    spdlog::debug("request full_path = {}", full_path);
    std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);

    if (!is) {
        spdlog::error("Can't open ifstream of request path. full_path = {}",
                      full_path);
        return -1;
    }

    // TODO need to add solution of different response status
    response.status = HTTP_STATUS_OK;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0) {
        response.content.append(buf, is.gcount());
    }
    // spdlog::debug("Content:\n{}", response.content);
    spdlog::debug("Content-Length: {0:d}", response.content.size());
    // template auto deduction
    response.headers.push_back(std::make_pair(
        "Content-Length", std::to_string(response.content.size())));
    response.headers.push_back(
        std::make_pair("Content-Type", extensionToType(extension)));

    return 0;
}

void Connection::doWrite() {
    auto self(shared_from_this());
    asio::async_write(
        socket_, response_.ToBuffers(),
        [this, self](asio::error_code ec, std::size_t num) {
            // TODO add some handle
            if (ec) {
                if (ec != asio::error::eof) {
                    spdlog::error("Client connection closed not graceful.{}",
                                  ec.message());
                }
            } else {
                // the request data is append, so must be clear every time
                // response data is also append, so it should be clear too
                reset();
                doRead();
            }
        });
}

void Connection::doWriteByStreamBuf() {
    std::ostream os(&write_buf_);
    os << response_;

    auto self(shared_from_this());
    asio::async_write(
        socket_, write_buf_,
        [this, self](asio::error_code ec, std::size_t num) {
            // TODO add some handle
            if (ec) {
                if (ec != asio::error::eof) {
                    spdlog::error("Client connection closed not graceful.{}",
                                  ec.message());
                }
            } else {
                spdlog::trace("Write back {:d} bytes to socket", num);
                // the request data is append, so must be clear every time
                // response data is also append, so it should be clear too
                reset();
                doRead();
            }
        });
}

}  // namespace Jebook