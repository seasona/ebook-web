#include "txt_book.h"
#include "iconv.h"
#include "spdlog/spdlog.h"
#include <regex>
#include <fstream>

namespace Jebook {

void TxtBook::setLangHint(Language lang) { lang_ = lang; }

void TxtBook::getEncode(std::fstream &fin) {
    // read 1024 bytes to detect the encoding of txt file
    char buf[4096];
    fin.read(buf, 1024);

    bool is_reliable;
    int bytes_consumed;

    Encoding encoding = CompactEncDet::DetectEncoding(
        buf, fin.gcount(), nullptr, nullptr, nullptr, UNKNOWN_ENCODING, lang_,
        CompactEncDet::QUERY_CORPUS, true, &bytes_consumed, &is_reliable);

    encode_ = encoding;
}

void convertToUTF8(std::fstream &fin, std::fstream &fout) {
    // the dest buf must be larger because change encoding may increase bytes of
    // file the input buf should be two part: |       4096 | 4096 | |the remain
    // bytes of last character|this time read bytes|
    char dest[4096 + 4096];
    char buf[4096 + 4096];
    int inbufrest = 0;

    iconv_t cd = iconv_open("UTF-8", "GB18030");

    while (fin.read(buf + 4096, 4096).gcount() > 0) {
        char *inptr = buf + 4096 - inbufrest;
        size_t leftsize = inbufrest + fin.gcount();

        char *outptr = dest;
        size_t outsize = sizeof(dest);
        size_t res = iconv(cd, &inptr, &leftsize, &outptr, &outsize);

        if (errno == EINVAL) {
            inbufrest = leftsize;
            if (leftsize > 0) {
                /* Like memcpy(inbuf+4096-insize,inptr,insize), except that
                    we cannot use memcpy here, because source and destination
                    regions may overlap. */
                /// move left character to the front of next operation
                char *restptr = buf + 4096 - leftsize;
                do {
                    *restptr++ = *inptr++;
                } while (--leftsize > 0);
            }
        }
        fout.write(dest, outptr - dest);
    }
}

std::string TxtBook::parse() {
    std::fstream fin, fout;
    fin.open(book_path_, std::fstream::in | std::fstream::binary);
    if (!fin.is_open()) {
        spdlog::error("Can't open txt book {}", book_path_);
    }

    getEncode(fin);
    fin.seekg(0);  // set the position indicator to the front of file

    std::string new_book_path;
    if (encode_ == CHINESE_GB) {
        // the txt ebook is encode by GB, should convert to UTF-8
        new_book_path = out_directory_ +
                        book_path_.substr(0, book_path_.find_last_of(".") - 1) +
                        "_utf.txt";
        spdlog::debug("Convert to encode UTF-8 file {}", new_book_path);
        fout.open(new_book_path, std::fstream::out);
        convertToUTF8(fin, fout);
        fout.close();
    } else if(encode_ == UTF8){
        new_book_path = book_path_;
    } else{
        spdlog::error("Don't support this encoding");
    }

    fin.close();

    //TODO regex should can be set
    std::regex chapter_regex("第[零一二三四五六七八九十百千万〇]+章");
    std::smatch base_match;
    std::string tmp;

    fin.open(new_book_path, std::fstream::in);
    if (!fin.is_open()) {
        spdlog::error("Can't open new ebook_path {}", new_book_path);
    }

    //TODO create toc.ncx xml for txt ebook
    //TODO create directory 
    while (std::getline(fin, tmp)) {
        // should use regex_search not regex_match
        if (std::regex_search(tmp, base_match, chapter_regex)) {
            fout.close();
            fout.open(base_match.str(), std::fstream::out);
        }
        if (fout.is_open()) {
            fout.write(tmp.c_str(), tmp.size());
        }
    }
}

}  // namespace Jebook