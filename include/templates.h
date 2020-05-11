#pragma once

#include "inja.hpp"
#include "nlohmann/json.hpp"
#include "xml2json.hpp"
#include <string>
#include <iostream>

namespace Jepub {

class Templates {
public:
    Templates() = default;
    ~Templates() = default;

    /**
     * @brief use inja to parse template file
     * @param ncx_path the directory ncx file
     * @param template_file the website template_file
     * @return std::string the string parsed
     */
    std::string parse(const std::string &ncx_path,
                      const std::string &template_path);

    // convert xml file to json
    void convert(const std::istream &ncx_path);
private:
    std::string ncx_path;
    std::string template_path;
    
    nlohmann::json ncx;
};

}  // namespace Jepub
