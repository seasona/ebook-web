#pragma once

#include <iostream>
#include <string>

#include "inja.hpp"
#include "nlohmann/json.hpp"
#include "xml2json.hpp"

namespace Jebook {

class Templates {
public:
    Templates() = default;
    Templates(const std::string& template_path,
              const std::string& result_directory, const std::string& book_name)
        : template_path_(template_path),
          result_directory_(result_directory),
          book_name_(book_name) {
        ncx_path_ = result_directory_ + "/OEBPS/toc.ncx";
    }
    ~Templates() = default;

    /**
     * @brief use inja to parse template file
     * @return std::string the string parsed
     */
    std::string parse();

    // convert xml file to json
    void convert();

    void setNcxPath(const std::string& ncx_path) { ncx_path_ = ncx_path; }

    const std::string& getNcxPath() { return ncx_path_; }

    void setTemplatePath(const std::string& template_path) {
        template_path_ = template_path;
    }

    const std::string& getTemplatePath() { return template_path_; }

private:
    std::string book_name_;
    std::string result_directory_;
    std::string ncx_path_;
    std::string template_path_;

    nlohmann::json ncx_;
};

}  // namespace Jebook
