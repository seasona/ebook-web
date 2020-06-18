#pragma once

#include "ebook.h"

namespace Jebook {

class MobiBook : public Ebook {
public:
    MobiBook(const std::string book_path, const std::string out_directory)
        : Ebook(book_path, out_directory) {}

    ~MobiBook() = default;

    virtual std::string parse() override;
};

}  // namespace Jebook