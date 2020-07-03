#include "server.h"
#include "spdlog/spdlog.h"
#include "epub_book.h"
#include "mobi_book.h"
#include "txt_book.h"
#include "cxxopts.hpp"
#include <iostream>
#include <memory>

cxxopts::ParseResult parse(int argc, char* argv[]) {
    try {
        cxxopts::Options options(argv[0], "Ebook reading CLI tool");
        options.positional_help("[optional args]").show_positional_help();

        options.allow_unrecognised_options().add_options()
            ("i,ip", "IP address", cxxopts::value<std::string>()->default_value("localhost"))
            ("p,port", "Port", cxxopts::value<std::string>()->default_value("8089"))
            ("d,directory", "Ebook output directory", cxxopts::value<std::string>()->default_value("./"))
            ("t,template", "Web template location", cxxopts::value<std::string>()->default_value("../../web/template.html"))
            ("b,book", "Ebook location", cxxopts::value<std::string>())
            ("help", "Print help")
            ;

        auto result = options.parse(argc, argv);

        if(result.count("help")){
            std::cout << options.help({""}) << std::endl;
            exit(0);
        }

        std::string ebook_path;
        std::string out_directory;
        std::string ip, port;

        if(!result.count("book")){
            spdlog::error("Please input the ebook path");
            exit(1);
        }else{
            ebook_path = result["book"].as<std::string>();
        }

        if(result.count("directory")){
            out_directory = result["directory"].as<std::string>();
        }else{
            out_directory = "./";
        }

        if(result.count("ip")){
            ip = result["ip"].as<std::string>();
        }else{
            ip = "localhost";
        }

        if(result.count("port")){
            port = result["port"].as<std::string>();
        }else{
            port = "8089";
        }

        std::unique_ptr<Jebook::Ebook> ebook;

        std::string file_type =
            ebook_path.substr(ebook_path.find_last_of(".") + 1);
        spdlog::trace("open ebook type is {}", file_type);
        if (file_type == "epub") {
            ebook =
                std::make_unique<Jebook::EpubBook>(ebook_path, out_directory);
        } else if (file_type == "mobi") {
            ebook =
                std::make_unique<Jebook::MobiBook>(ebook_path, out_directory);
        } else if (file_type == "azw3") {
            ebook =
                std::make_unique<Jebook::MobiBook>(ebook_path, out_directory);
        } else if (file_type == "txt") {
            ebook =
                std::make_unique<Jebook::TxtBook>(ebook_path, out_directory);
        }

        std::string result_directory = ebook->parse();

        // TODO: support epub2
        std::string oepbs = result_directory + "/OEBPS";
        spdlog::info("Ebook parsed result path is {}", oepbs);

        Jebook::Server server(ip.c_str(), port.c_str(), oepbs.c_str());

        if(result.count("template")){
            auto& t = result["template"].as<std::string>();
            server.setTemplatePath(t);
        }else{
            server.setTemplatePath("../../web/template.html");
        }

        server.run();

        return result;

    } catch (const cxxopts::OptionException& e) {
        std::cout << "error parsing options: " << e.what() << std::endl;
        exit(1);
    }
}

int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::info);
    auto result = parse(argc, argv);
    return 0;
}