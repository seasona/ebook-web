#include "ebook.h"
#include "spdlog/spdlog.h"
#include <fstream>

namespace Jebook {
    
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