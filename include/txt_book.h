#pragma once
#include "ebook.h"
#include "compact_enc_det/compact_enc_det.h"

namespace Jebook {

class TxtBook : public Ebook {
public:
    TxtBook(const std::string book_path, const std::string out_directory)
        : Ebook(book_path, out_directory) {}

    ~TxtBook() = default;

    virtual std::string parse() override;

    void setLangHint(Language lang);

private:
    void getEncode(std::fstream& fin);

    // convert to utf-8 
    void convertToUTF8(std::fstream& fin, std::fstream& fout);

    Encoding encode_;

    Language lang_ = Language::CHINESE;

};

}  // namespace Jebook
