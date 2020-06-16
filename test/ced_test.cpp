#include "compact_enc_det/compact_enc_det.h"
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    std::fstream fs;
    fs.open(argv[1], std::fstream::in | std::fstream::binary);
    if (!fs.is_open()) {
        std::cout << "Can't open source file: " << argv[1] << std::endl;
        return -1;
    }

    char buf[4096];
    fs.read(buf, 1024);

    bool is_reliable;
    int bytes_consumed;

    Encoding encoding = CompactEncDet::DetectEncoding(
        buf, fs.gcount(), nullptr, nullptr, nullptr, UNKNOWN_ENCODING,
        Language::CHINESE, CompactEncDet::QUERY_CORPUS, true, &bytes_consumed,
        &is_reliable);

    printf("%d\n", encoding);

    return 0;
}