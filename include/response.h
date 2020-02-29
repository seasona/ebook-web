#pragma once
#include "http_status_def.h"
#include "boost/asio.hpp"
#include <vector>
#include <iostream>

namespace Jhttp {

struct Response {
    int status = HTTP_STATUS_OK;
    unsigned short version_major = 1;
    unsigned short version_minor = 1;
    std::string content;
    std::vector<std::pair<std::string, std::string>> headers;
    std::vector<boost::asio::const_buffer> ToBuffers();
};

std::ostream& operator<<(std::ostream& os, const Response& response);

}  // namespace Jhttp