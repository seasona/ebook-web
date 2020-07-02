#include "server.h"
#include "spdlog/spdlog.h"
#include "epub_book.h"
#include "mobi_book.h"
#include "txt_book.h"
#include <iostream>
#include <memory>

int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::debug);
    try {
        if (argc != 5) {
            std::cerr << "Usage: ebook-web <address> <port> <ebook_path> "
                         "<out_directory>"
                      << std::endl;
            return 1;
        }

        std::string ebook_path(argv[3]);
        std::string out_directory(argv[4]);

        std::unique_ptr<Jebook::Ebook> ebook;

        std::string file_type =
            ebook_path.substr(ebook_path.find_last_of(".") + 1);
        spdlog::trace("open ebook type is {}", file_type);
        if (file_type == "epub") {
            ebook = std::make_unique<Jebook::EpubBook>(ebook_path, out_directory);
        } else if (file_type == "mobi") {
            ebook = std::make_unique<Jebook::MobiBook>(ebook_path, out_directory);
        } else if (file_type == "azw3") {
            ebook = std::make_unique<Jebook::MobiBook>(ebook_path, out_directory);
        } else if (file_type == "txt") {
            ebook = std::make_unique<Jebook::TxtBook>(ebook_path, out_directory);
        }

        std::string result_directory = ebook->parse();

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