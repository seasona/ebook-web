#include "compact_enc_det/compact_enc_det.h"
#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <iostream>

std::string utf8_long;

std::string utf8_short =
    "\xe5\x9c\xa8\xe8\xbf\x99\xe6\xa0\xb7\xe9\x9b\xa8\xe9\x9b\xaa\xe4\xba\xa4"
    "\xe5\x8a\xa0\xe7\x9a\x84\xe6\x97\xa5\xe5\xad\x90\xe9\x87\x8c\xef\xbc\x8c"
    "\xe5\xa6\x82\xe6\x9e\x9c\xe6\xb2\xa1\xe6\x9c\x89\xe4\xbb\x80\xe4\xb9\x88"
    "\xe7";

std::string GB18030_short =
    "\xD6\xBB\xD3\xD0\xD4\xDA\xB0\xEB\xC9\xBD\xD1\xFC\xCF\xD8\xC1\xA2\xB8\xDF"
    "\xD6\xD0\xB5\xC4\xB4\xF3\xD4\xBA\xB0\xD3\xC0\xEF\xA3\xAC\xB4\xCB\xBF\xCC"
    "\xC8\xB4\xD7\xD4\xD3\xD0\xD2\xBB\xB7\xAC\xC8\xC8\xC4\xD6\xBE\xB0\xCF\xF3";

TEST(UTF8ShortTest, CEDTest) {
    bool is_reliable;
    int bytes_consumed;

    Encoding encoding = CompactEncDet::DetectEncoding(
        utf8_short.c_str(), utf8_short.size(), nullptr, nullptr, nullptr,
        UNKNOWN_ENCODING, Language::CHINESE, CompactEncDet::QUERY_CORPUS, true,
        &bytes_consumed, &is_reliable);

    EXPECT_EQ(encoding, Encoding::UTF8);
}

TEST(GB18030ShortTest, CEDTest){
    bool is_reliable;
    int bytes_consumed;

    Encoding encoding = CompactEncDet::DetectEncoding(
        GB18030_short.c_str(), GB18030_short.size(), nullptr, nullptr, nullptr,
        UNKNOWN_ENCODING, Language::CHINESE, CompactEncDet::QUERY_CORPUS, true,
        &bytes_consumed, &is_reliable);

    // normally, CHINESE_GB is the default GB encoding
    EXPECT_EQ(encoding, Encoding::CHINESE_GB);
}
