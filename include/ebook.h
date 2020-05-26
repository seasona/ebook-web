#pragma once

#include "mobi.h"
#include "zipper.h"
#include <string>

namespace Jebook {

enum EbookType {
    EBOOK_EPUB = 0,
    EBOOK_MOBI = 1,
    EBOOK_AZW3 = 2,
    EBOOK_TXT = 3
};

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

    // get ebook type
    EbookType getType();

    /**
     * @brief parse the ebook and return the parsed directory
     * @return std::string the parsed directory of ebook
     */
    std::string parseEbook();

private:
    // parse mobi and azw3 file and create a file structure like epub
    std::string parseMobi();

    // parse epub file, basically is unzip the zip file
    std::string parseEpub();

    // parse normal txt book
    std::string parseTxt();

    bool writeIntoFile(std::fstream &fs, const std::string &result_path,
                          const char *buf, size_t size);

    const std::string book_path_;

    const std::string out_directory_;

    Zipper zipper_;
};

}  // namespace Jebook