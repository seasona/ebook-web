#include "server.h"
#include "spdlog/spdlog.h"
#include "ebook.h"
#include <iostream>

int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::debug);
    try {
        if (argc != 5) {
            std::cerr << "Usage: http_server <address> <port> <ebook_path> <out_directory>"
                      << std::endl;
            return 1;
        }

        std::string ebook_path(argv[3]);
        std::string out_directory(argv[4]);
        Jebook::Ebook ebook(ebook_path, out_directory);
        std::string result_directory = ebook.parseEbook();
        
        // TODO: support epub2
        std::string oepbs = result_directory + "/OEBPS";
        spdlog::info("Ebook parsed result path is {}", oepbs);
        
        Jebook::Server s(argv[1], argv[2], oepbs.c_str());
        s.setTemplatePath("../../web/template.html");
        s.run();

    } catch (std::exception& e) {
        std::cerr << "The exception: " << e.what() << std::endl;
    }

    return 0;
}