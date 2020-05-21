#pragma once
#include <string>

namespace Jebook{

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

private:
    bool isExist(const std::string& path);

    bool isReadable(const std::string& path);

    bool isWritable(const std::string& path);

    bool isDir(const std::string& path);

    std::string normalize(const std::string& path);

    std::string getDirName(const std::string& path);

    bool createDir(const std::string& dir, const std::string& parent = "");
};

} // namespace Jebook;