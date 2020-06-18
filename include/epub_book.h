#pragma once

#include "ebook.h"

namespace Jebook {

class EpubBook : public Ebook {
public:
    EpubBook(const std::string book_path, const std::string out_directory)
        : Ebook(book_path, out_directory) {}

    ~EpubBook() = default;
    
    virtual std::string parse() override;

};

}  // namespace Jebook