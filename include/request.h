#pragma once

#include <string>
#include <vector>

namespace Jhttp {

struct Request {
    std::string method;
    std::string uri;
    int version_major;
    int version_minor;
    //? maybe need fix
    std::vector<std::string> header_field;
    std::vector<std::string> header_value;
    std::string body;
};

}  // namespace Jhttp
