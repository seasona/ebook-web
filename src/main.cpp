#include "server.h"
#include "spdlog/spdlog.h"
#include "zipper.h"
#include <iostream>

int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::info);
    try {
        if (argc != 4) {
            std::cerr << "Usage: http_server <address> <port> <epub_path>"
                      << std::endl;
            return 1;
        }

        Jepub::Zipper zipper;
        zipper.extractToFile(argv[3]);

        Jhttp::Server s(argv[1], argv[2], argv[3]);
        s.run();

    } catch (std::exception& e) {
        std::cerr << "The exception: " << e.what() << std::endl;
    }

    return 0;
}