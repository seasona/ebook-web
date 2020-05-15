#include "templates.h"
#include "spdlog/spdlog.h"

namespace Jepub {

void Templates::convert() {  
    auto ncx_path = std::ifstream(ncx_path_);
    if(!ncx_path.is_open()){
        spdlog::error("Can't open ncx_path");
    }
    std::ostringstream oss;
    oss << ncx_path.rdbuf();

    const auto json_str = xml2json(oss.str().data());
    ncx_ = nlohmann::json::parse(json_str);
}

std::string Templates::parse() {
    this->convert();

    inja::Environment env;
    env.set_trim_blocks(true);
    env.set_lstrip_blocks(true);
    // inja has its own error throw handler
    std::string result = env.render_file(template_path_, this->ncx_);
    return result;
}

}  // namespace Jepub