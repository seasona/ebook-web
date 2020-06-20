#include <regex>
#include <fstream>
#include <iostream>
#include <string>
#include "unicode/ucsdet.h"
#include "unicode/uclean.h"
#include "unicode/ucnv.h"
#include "unicode/utypes.h"

ssize_t io_read_utf8_string(const char *utf8, size_t utf8len, UChar *buf) {
    const char *source;
    const char *sourceLimit;
    UChar *target;
    UChar *targetLimit;

    source = utf8;
    sourceLimit = utf8 + utf8len;

    target = buf;
    targetLimit = buf + utf8len;

    UErrorCode uerr = U_ZERO_ERROR;
    UConverter *uconv = ucnv_open("GB18030", &uerr);

    ucnv_toUnicode(uconv, &target, targetLimit, &source, sourceLimit, NULL,
                   FALSE, &uerr);

    if (uerr == U_BUFFER_OVERFLOW_ERROR) {
        fprintf(stderr,
                "ucnv_toUnicode() in io_read_utf8_string ran out of target "
                "buffer when converting from utf8\n");
        uerr = U_ZERO_ERROR;
    }
    printf("source read %d bytes, target get %d bytes.\n", source - utf8,
           target - buf);
    return (ssize_t)(target - buf);
}

int main(int argc, char **argv) {
    std::fstream fs;
    fs.open(argv[1], std::fstream::in | std::fstream::binary);
    if (!fs.is_open()) {
        std::cout << "Can't open source file: " << argv[1] << std::endl;
        return -1;
    }

    char buf[4096];
    // std::string buf;
    fs.read(buf, 512);

    UErrorCode uerr = U_ZERO_ERROR;
    UCharsetDetector *ucd = ucsdet_open(&uerr);
    ucsdet_setText(ucd, buf, 512, &uerr);

    UCharsetMatch const *match = ucsdet_detect(ucd, &uerr);

    if (ucsdet_getConfidence(match, &uerr) > 50) {
        printf("Name: %s\n", ucsdet_getName(match, &uerr));
        printf("Lang: %s\n", ucsdet_getLanguage(match, &uerr));
    } else {
        printf("Error: can't decide the encoding of txt\n");
        return -1;
    }

    // the ICU default use UTF-16, pass -DU_CHARSET_IS_UTF8=1 to set UTF-8 as
    // default
    if (std::strcmp(ucsdet_getName(match, &uerr), "GB18030") == 0) {
        UConverter *uconv = ucnv_open("GB18030", &uerr);
        fs.seekg(0);
        UChar dest[4096 + 4096];
        std::fstream fout;
        fout.open(argv[2], std::fstream::out);
        if (!fout.is_open()) {
            std::cout << "Can't open destination file: " << argv[2]
                      << std::endl;
            return -1;
        }
        bool flag = true;
        int cnt = 2;
        // must be sure the read size can be complete chinese character
        while (fs.read(buf, 4096).gcount() > 0) {
            auto size = fs.gcount();

            //! the convert may increase the bytes amount, so the size of dest
            //! should be larger
            if (cnt-- > 0) {
                auto dest_cnt = io_read_utf8_string(buf, size, dest);
                for (int i = 0; i < dest_cnt; i++) {
                    std::wcout << dest[i];
                }
            }
            // fout.write(dest, dest_cnt);
        }
    }

    ucsdet_close(ucd);
    u_cleanup();  // keep valgrind happy!!

    // // char tmp[10000];
    std::string tmp;
    std::fstream fout;
    std::regex chapter_regex("第[零一二三四五六七八九十百千万]+章");
    std::smatch base_match;
    int col = 1000;

    while (std::getline(fs, tmp) && col--) {
        // should use regex_search not regex_match
        if (std::regex_search(tmp, base_match, chapter_regex)) {
            printf("Catch!\n");
            fout.close();
            fout.open(base_match.str(), std::fstream::out);
        }
        if (fout.is_open()) {
            fout.write(tmp.c_str(), tmp.size());
        }
    }
    return 0;
}