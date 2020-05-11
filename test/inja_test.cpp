#include "inja.hpp"
#include "nlohmann/json.hpp"
#include "xml2json.hpp"
#include <string>
#include <iomanip>
#include <iostream>

std::string convert(const std::istream &input) {
    std::ostringstream oss;
    oss << input.rdbuf();

    const auto json_str = xml2json(oss.str().data());
    return json_str;
}

int main(const int argc, const char *const argv[]) {
    std::string json_str = convert(std::ifstream(argv[1]));
    nlohmann::json data = nlohmann::json::parse(json_str);
    // for (auto &dic1 : data["ncx"]["navMap"]["navPoint"]) {
    //     std::cout << dic1["navLabel"]["text"] << std::endl;
    //     for (auto &dic2 : dic1["navPoint"]) {
    //         std::cout << "--" << dic2["navLabel"]["text"];
    //         std::cout << ": " << dic2["content"]["src"] << std::endl;
    //     }
    // }

    inja::Environment env;
    env.set_trim_blocks(true);
    env.set_lstrip_blocks(true);
    std::string result = env.render_file("../../web/template.html", data);
    std::cout << result << std::endl;

    return 0;
}