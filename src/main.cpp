#include "server.h"
#include "spdlog/spdlog.h"
#include "zipper.h"
#include <iostream>

int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::debug);
    try {
        if (argc != 4) {
            std::cerr << "Usage: http_server <address> <port> <epub_path>"
                      << std::endl;
            return 1;
        }

        Jebook::Zipper zipper;
        std::string unzip_path = zipper.extractToFile(argv[3]);
        // TODO: support epub2
        std::string epub_oepbs = unzip_path + "/OEBPS";
        
        Jebook::Server s(argv[1], argv[2], epub_oepbs.c_str());
        s.setTemplatePath("../../web/template.html");
        s.run();

    } catch (std::exception& e) {
        std::cerr << "The exception: " << e.what() << std::endl;
    }

    return 0;
}