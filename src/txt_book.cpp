#include "txt_book.h"
#include "iconv.h"
#include "spdlog/spdlog.h"
#include "libxml/xmlwriter.h"
#include <regex>
#include <fstream>
#include <iostream>

namespace Jebook {

void TxtBook::setLangHint(Language lang) { lang_ = lang; }

void TxtBook::setRegex(std::string regex) { regex_ = regex; }

std::string static getPathName(const std::string &book_path) {
    auto begin_index = book_path.find_last_of(k_separator);
    if (begin_index == std::string::npos) {
        begin_index = 0;
    } else {
        begin_index++;
    }
    auto end_index = book_path.find_last_of(".") - 1;
    return book_path.substr(begin_index, end_index - begin_index + 1);
}

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

void TxtBook::convertToUTF8(std::fstream &fin, std::fstream &fout) {
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
                do { *restptr++ = *inptr++; } while (--leftsize > 0);
            }
        }
        fout.write(dest, outptr - dest);
    }
}

void TxtBook::createNcx(std::string ncx_path) {
    std::fstream fout;
    char *ncx_content;
    const xmlChar *NCXNamespace =
        BAD_CAST "http://www.daisy.org/z3986/2005/ncx/";
    xmlBufferPtr buf = xmlBufferCreate();
    if (buf == NULL) {
        spdlog::error("Memory allocation failed when create ncx");
        return;
    }
    xmlTextWriterPtr writer = xmlNewTextWriterMemory(buf, 0);
    if (writer == NULL) {
        xmlBufferFree(buf);
        spdlog::error("Memory allocation failed when create ncx");
        return;
    }
    xmlTextWriterSetIndent(writer, 1);

    // <ncx xmlns="http://www.daisy.org/z3986/2005/ncx/" version="2005-1"
    // xml:lang="zho">
    int xml_ret = xmlTextWriterStartDocument(writer, NULL, NULL, NULL);
    if (xml_ret < 0) { goto cleanup; }
    xml_ret =
        xmlTextWriterStartElementNS(writer, NULL, BAD_CAST "ncx", NCXNamespace);
    if (xml_ret < 0) { goto cleanup; }
    xml_ret = xmlTextWriterWriteAttribute(writer, BAD_CAST "version",
                                          BAD_CAST "2005-1");
    if (xml_ret < 0) { goto cleanup; }

    // start <docTitle>
    xml_ret = xmlTextWriterStartElement(writer, BAD_CAST "docTitle");
    if (xml_ret < 0) { goto cleanup; }

    xml_ret = xmlTextWriterStartElement(writer, BAD_CAST "text");
    if (xml_ret < 0) { goto cleanup; }

    xml_ret = xmlTextWriterWriteString(
        writer, BAD_CAST getPathName(book_path_).c_str());
    if (xml_ret < 0) { goto cleanup; }

    xml_ret = xmlTextWriterEndElement(writer);
    if (xml_ret < 0) { goto cleanup; }

    xml_ret = xmlTextWriterEndElement(writer);  // end <docTitle>
    if (xml_ret < 0) { goto cleanup; }

    /* start <navMap> */
    xml_ret = xmlTextWriterStartElement(writer, BAD_CAST "navMap");
    if (xml_ret < 0) { goto cleanup; }

    for (int i = 0; i < chapters_.size(); i++) {
        xml_ret = xmlTextWriterStartElement(writer, BAD_CAST "navPoint");
        if (xml_ret < 0) { goto cleanup; }
        xml_ret = xmlTextWriterWriteAttribute(writer, BAD_CAST "class",
                                              BAD_CAST "chapter");
        if (xml_ret < 0) { goto cleanup; }

        std::string play_order = std::to_string(i + 1);
        std::string id = "num_" + play_order;
        xml_ret = xmlTextWriterWriteAttribute(writer, BAD_CAST "id",
                                              BAD_CAST id.c_str());
        if (xml_ret < 0) { goto cleanup; }
        xml_ret = xmlTextWriterWriteAttribute(writer, BAD_CAST "playOrder",
                                              BAD_CAST play_order.c_str());
        if (xml_ret < 0) { goto cleanup; }

        xml_ret = xmlTextWriterStartElement(writer, BAD_CAST "navLabel");
        if (xml_ret < 0) { goto cleanup; }
        xml_ret = xmlTextWriterStartElement(writer, BAD_CAST "text");
        if (xml_ret < 0) { goto cleanup; }
        xml_ret =
            xmlTextWriterWriteString(writer, BAD_CAST chapters_[i].c_str());
        if (xml_ret < 0) { goto cleanup; }
        xml_ret = xmlTextWriterEndElement(writer);  // end <text>
        if (xml_ret < 0) { goto cleanup; }
        xml_ret = xmlTextWriterEndElement(writer);  // end <navLabel>
        if (xml_ret < 0) { goto cleanup; }

        xml_ret = xmlTextWriterStartElement(writer, BAD_CAST "content");
        if (xml_ret < 0) { goto cleanup; }

        xml_ret = xmlTextWriterWriteAttribute(
            writer, BAD_CAST "src", BAD_CAST("Text/" + chapters_[i]).c_str());
        if (xml_ret < 0) { goto cleanup; }
        xml_ret = xmlTextWriterEndElement(writer);  // end <content>
        if (xml_ret < 0) { goto cleanup; }

        xml_ret = xmlTextWriterEndElement(writer);  // end <navPoint>
        if (xml_ret < 0) { goto cleanup; }
    }
    /* end <navMap> */
    xml_ret = xmlTextWriterEndDocument(writer);
    if (xml_ret < 0) { goto cleanup; }

    fout.open(ncx_path, std::fstream::out);
    if (!fout.is_open()) {
        spdlog::error("Can't open the ncx file: {}", ncx_path);
    }

    //? the buf->use is useless, that's strange, there are some problem
    ncx_content = reinterpret_cast<char *>(buf->content);
    fout.write(ncx_content, strlen(ncx_content));

    xmlFreeTextWriter(writer);
    xmlBufferFree(buf);
    return;

cleanup:
    xmlFreeTextWriter(writer);
    xmlBufferFree(buf);
    spdlog::error("XML writing failed");
}

std::string TxtBook::parse() {
    std::fstream fin, fout;
    std::string book_name;
    fin.open(book_path_, std::fstream::in | std::fstream::binary);
    if (!fin.is_open()) { spdlog::error("Can't open txt book {}", book_path_); }

    getEncode(fin);
    fin.seekg(0);  // set the position indicator to the front of file

    book_name = getPathName(book_path_);

    std::string new_book_path;
    if (encode_ == CHINESE_GB) {
        // the txt ebook is encode by GB, should convert to UTF-8
        new_book_path =
            zipper_.normalize(out_directory_ + k_separator + book_name +
                              k_separator + book_name + "_utf.txt");
        spdlog::debug("Convert to encode UTF-8 file {}", new_book_path);
        zipper_.createDir(zipper_.getDirName(new_book_path));
        fout.open(new_book_path, std::fstream::out);
        convertToUTF8(fin, fout);
        fout.close();
    } else if (encode_ == UTF8) {
        new_book_path = book_path_;
    } else {
        spdlog::error("Don't support this encoding");
    }

    fin.close();

    //? the regex in cpp is strange, must have a `+` after `[]`
    std::regex chapter_regex(regex_);
    std::smatch base_match;
    std::string tmp;

    fin.open(new_book_path, std::fstream::in);
    if (!fin.is_open()) {
        spdlog::error("Can't open new ebook_path {}", new_book_path);
    }

    // create directory
    std::string new_dir =
        zipper_.normalize(out_directory_ + k_separator + book_name);

    std::string text_dir =
        new_dir + k_separator + "OEBPS" + k_separator + "Text";
    spdlog::debug("the book name is {}, new dir is {}", book_name, new_dir);
    zipper_.createDir(text_dir);

    while (std::getline(fin, tmp)) {
        // should use regex_search not regex_match
        if (std::regex_search(tmp, base_match, chapter_regex)) {
            fout.close();
            fout.open(text_dir + k_separator + base_match.str(),
                      std::fstream::out);
            chapters_.push_back(base_match.str());
        }
        if (fout.is_open()) { fout.write(tmp.c_str(), tmp.size()); }
    }

    std::string ncx_path =
        new_dir + k_separator + "OEBPS" + k_separator + "toc.ncx";
    spdlog::debug("Ncx path is {}", ncx_path);
    createNcx(ncx_path);

    return new_dir;
}

}  // namespace Jebook