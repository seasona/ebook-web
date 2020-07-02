#pragma once
#include "ebook.h"
#include "compact_enc_det/compact_enc_det.h"
#include <vector>

namespace Jebook {

class TxtBook : public Ebook {
public:
    TxtBook(const std::string book_path, const std::string out_directory)
        : Ebook(book_path, out_directory) {}

    ~TxtBook() = default;

    virtual std::string parse() override;

    void setLangHint(Language lang);

    void setRegex(std::string regex);

private:
    void getEncode(std::fstream& fin);

    // convert to utf-8
    void convertToUTF8(std::fstream& fin, std::fstream& fout);

    void createNcx(std::string ncx_path);

    Encoding encode_;

    Language lang_ = Language::CHINESE;

    std::string regex_ = "(第|卷)[0123456789一二三四五六七八九十零〇百千两]+(章|回|部|节|集|卷) .*";

    std::vector<std::string> chapters_;
};

}  // namespace Jebook
