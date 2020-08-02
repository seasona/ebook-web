#include "ebook.h"
#include "spdlog/spdlog.h"
#include <fstream>

namespace Jebook {

std::string Ebook::getBookName(const std::string& book_path){
    // TODO need special deal with input like './'
    std::string base_name =
        book_path.substr(book_path.find_last_of("/\\") + 1);
    std::string book_name = base_name.substr(0, base_name.find_last_of("."));

    return book_name;
}
    
bool Ebook::writeIntoFile(std::fstream &fs, const std::string &result_path,
                          const char *buf, size_t size) {
    std::string dir = zipper_.getDirName(result_path);
    bool ret = zipper_.createDir(dir);
    if (!ret) {
        spdlog::error("create directory {} failed", dir);
    }

    fs.open(result_path, std::fstream::out);
    if (!fs.is_open()) {
        spdlog::error("open file {} failed", result_path);
        return false;
    } else {
        fs.write(buf, size);
        fs.close();
    }
    return true;
}

}  // namespace Jebook