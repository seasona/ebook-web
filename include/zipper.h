#pragma once
#include <string>

namespace Jebook {

#ifdef WIN32
const std::string k_separator = "\\";
#else
const std::string k_separator = "/";
#endif

class Zipper {
public:
    Zipper() = default;
    ~Zipper() = default;
    Zipper(const Zipper&) = delete;
    Zipper& operator=(const Zipper&) = delete;

    /**
     * @brief extract zip file to local path
     * @param file_path the zip file path
     * @param out_directory set extracted file to this directory
     */
    std::string extractToFile(const std::string& file_path,
                              const std::string& out_directory = "");

    bool createDir(const std::string& dir, const std::string& parent = "");

    std::string getDirName(const std::string& path);

private:
    bool isExist(const std::string& path);

    bool isReadable(const std::string& path);

    bool isWritable(const std::string& path);

    bool isDir(const std::string& path);

    std::string normalize(const std::string& path);
};

}  // namespace Jebook