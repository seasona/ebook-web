#include "epub_book.h"

namespace Jebook{

std::string EpubBook::parse(){
    // TODO need special deal with input like './'
    std::string base_name =
        book_path_.substr(book_path_.find_last_of("/\\") + 1);
    std::string book_name = base_name.substr(0, base_name.find_last_of("."));
    std::string dir_name;

    if (out_directory_.empty()) {
        dir_name = book_name;
    } else {
        if (out_directory_.back() == '/') {
            dir_name = out_directory_ + book_name;
        } else {
            dir_name = out_directory_ + k_separator + book_name;
        }
    }
    std::string unzip_path = zipper_.extractToFile(book_path_, dir_name);
    return unzip_path;
}

}// namespace Jebook