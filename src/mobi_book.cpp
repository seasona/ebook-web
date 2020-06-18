#include "mobi_book.h"
#include "mobi.h"
#include "spdlog/spdlog.h"
#include <fstream>

namespace Jebook {

#define EPUB_CONTAINER \
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<container version=\"1.0\" xmlns=\"urn:oasis:names:tc:opendocument:xmlns:container\">\n\
  <rootfiles>\n\
    <rootfile full-path=\"OEBPS/content.opf\" media-type=\"application/oebps-package+xml\"/>\n\
  </rootfiles>\n\
</container>"
#define EPUB_MIMETYPE "application/epub+zip"

std::string MobiBook::parse() {
    /* Initialize main MOBIData structure */
    /* Must be deallocated with mobi_free() when not needed */
    MOBIData *m = mobi_init();
    if (m == NULL) {
        spdlog::error("MOBIData initialize failed");
    }

    /* Open file for reading */
    FILE *file = fopen(book_path_.data(), "rb");
    if (file == NULL) {
        mobi_free(m);
        spdlog::error("Can't open mobi book {}", book_path_);
    }

    /* Load file into MOBIData structure */
    /* This structure will hold raw data/metadata from mobi document */
    MOBI_RET mobi_ret = mobi_load_file(m, file);
    fclose(file);
    if (mobi_ret != MOBI_SUCCESS) {
        mobi_free(m);
        spdlog::error("Load mobi book failed");
    }

    /* Initialize MOBIRawml structure */
    /* Must be deallocated with mobi_free_rawml() when not needed */
    /* In the next step this structure will be filled with parsed data */
    MOBIRawml *rawml = mobi_init_rawml(m);
    if (rawml == NULL) {
        mobi_free(m);
        spdlog::error("MOBIRawml initialize failed");
    }
    /* Raw data from MOBIData will be converted to html, css, fonts, media
     * resources */
    /* Parsed data will be available in MOBIRawml structure */
    mobi_ret = mobi_parse_rawml(rawml, m);
    if (mobi_ret != MOBI_SUCCESS) {
        mobi_free(m);
        mobi_free_rawml(rawml);
        spdlog::error("Mobi book parse failed");
    }

    std::string base_name =
        book_path_.substr(book_path_.find_last_of("/\\") + 1);
    std::string book_name = base_name.substr(0, base_name.find_last_of("."));
    std::string dir_name;

    // TODO need special deal with input like './'
    if (out_directory_.empty()) {
        dir_name = book_name;
    } else {
        if (out_directory_.back() == '/') {
            dir_name = out_directory_ + book_name;
        } else {
            dir_name = out_directory_ + k_separator + book_name;
        }
    }

    std::string result_path;
    std::fstream fs;

    // write mimetype into file
    result_path = dir_name + k_separator + "mimetype";
    writeIntoFile(fs, result_path, EPUB_MIMETYPE, sizeof(EPUB_MIMETYPE) - 1);

    // write container.xml into file
    result_path = dir_name + k_separator + "META-INF/container.xml";
    writeIntoFile(fs, result_path, EPUB_CONTAINER, sizeof(EPUB_CONTAINER) - 1);

    char partname[4096];
    // Linked list of MOBIPart structures in rawml->markup holds main text files
    if (rawml->markup != NULL) {
        MOBIPart *curr = rawml->markup;
        while (curr != NULL) {
            MOBIFileMeta file_meta = mobi_get_filemeta_by_type(curr->type);
            snprintf(partname, sizeof(partname), "OEBPS/part%05zu.%s",
                     curr->uid, file_meta.extension);
            std::string result_path = dir_name + k_separator + partname;
            bool ret = writeIntoFile(fs, result_path, (const char *)curr->data,
                                     curr->size);
            if (!ret) {
                spdlog::error("Could not add file to archive: {}", result_path);
            }
            curr = curr->next;
        }
    }

    // Linked list of MOBIPart structures in rawml->flow holds format css files
    if (rawml->flow != NULL) {
        MOBIPart *curr = rawml->flow;
        /* skip raw html file */
        curr = curr->next;
        while (curr != NULL) {
            MOBIFileMeta file_meta = mobi_get_filemeta_by_type(curr->type);
            snprintf(partname, sizeof(partname), "OEBPS/flow%05zu.%s",
                     curr->uid, file_meta.extension);
            std::string result_path = dir_name + k_separator + partname;
            bool ret = writeIntoFile(fs, result_path, (const char *)curr->data,
                                     curr->size);
            if (!ret) {
                spdlog::error("Could not add file to archive: {}", result_path);
            }
            curr = curr->next;
        }
    }

    // Linked list of MOBIPart structures in rawml->resources holds binary files
    // like pictures and audio, also include opf and ncx file
    if (rawml->resources != NULL) {
        MOBIPart *curr = rawml->resources;
        /* jpg, gif, png, bmp, font, audio, video, also opf, ncx */
        while (curr != NULL) {
            MOBIFileMeta file_meta = mobi_get_filemeta_by_type(curr->type);
            if (curr->size > 0) {
                if (file_meta.type == T_OPF) {
                    snprintf(partname, sizeof(partname), "OEBPS/content.opf");
                } else if (file_meta.type == T_NCX) {
                    snprintf(partname, sizeof(partname), "OEBPS/toc.ncx");
                } else {
                    snprintf(partname, sizeof(partname),
                             "OEBPS/resource%05zu.%s", curr->uid,
                             file_meta.extension);
                }
                std::string result_path = dir_name + k_separator + partname;
                bool ret = writeIntoFile(fs, result_path,
                                         (const char *)curr->data, curr->size);
                if (!ret) {
                    spdlog::error("Could not add file to archive: {}",
                                  result_path);
                }
            }
            curr = curr->next;
        }
    }

    /* Free MOBIRawml structure */
    mobi_free_rawml(rawml);

    /* Free MOBIData structure */
    mobi_free(m);

    return dir_name;
}

}  // namespace Jebook