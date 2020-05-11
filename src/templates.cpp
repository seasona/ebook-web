#include "templates.h"

namespace Jepub {

void Templates::convert(const std::istream &ncx_path) {
    std::ostringstream oss;
    oss << ncx_path.rdbuf();

    const auto json_str = xml2json(oss.str().data());
    ncx = nlohmann::json::parse(json_str);
}

std::string Templates::parse(const std::string &ncx_path,
                             const std::string &template_path) {
    convert(std::ifstream(ncx_path));
    
    inja::Environment env;
    env.set_trim_blocks(true);
    env.set_lstrip_blocks(true);
    std::string result = env.render_file(template_path, this->ncx);
    return result;
}

}  // namespace Jepub