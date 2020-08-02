#pragma once

#include "zipper.h"
#include <string>

namespace Jebook {

/**
 * @brief handle epub, mobi and azw3
 */
class Ebook {
public:
    // TODO try to use std::string
    Ebook(const std::string book_path,
          const std::string out_directory)
        : book_path_(book_path), out_directory_(out_directory){};
    ~Ebook() = default;

    /**
     * @brief parse the ebook and return the parsed directory
     * @return std::string the parsed directory of ebook
     */
    virtual std::string parse() = 0;

    std::string getBookName(const std::string& book_path);

protected:
    bool writeIntoFile(std::fstream &fs, const std::string &result_path,
                          const char *buf, size_t size);

    const std::string book_path_;

    const std::string out_directory_;

    Zipper zipper_;
};

}  // namespace Jebook