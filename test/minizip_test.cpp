// Test the minizip and wrapper to extract files from zip to local

#include "unzip.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#include <io.h>
#include <direct.h>
typedef struct _stat STAT;
#define stat _stat
#define S_IFREG _S_IFREG
#define S_IFDIR _S_IFDIR
#define access _access
#define mkdir _mkdir
#define rmdir _rmdir
#else
typedef struct stat STAT;
#include <dirent.h>
#include <unistd.h>
#endif

#ifdef WIN32
const std::string k_separator = "\\";
#else
const std::string k_separator = "/";
#endif

const int max_path_length = 256;
const int buffer_length = 8192;
const char* output_directory = "/home/season/Jhttp_server/data/trival/";
const char* epub_path = "../../data/trival.epub";

bool exist(const std::string& path) {
    STAT st;

    if (stat(path.c_str(), &st) == -1) return false;

#ifdef WIN32
    return ((st.st_mode & S_IFREG) == S_IFREG ||
            (st.st_mode & S_IFDIR) == S_IFDIR);
#else
    // S_ISREG: is a regular file?
    // S_ISDIR：is a directory?
    return (S_ISREG(st.st_mode) || S_ISDIR(st.st_mode));
#endif
}

bool isDir(const std::string& path) {
    STAT st;

    if (stat(path.c_str(), &st) == -1) return false;

#ifdef WIN32
    return ((st.st_mode & S_IFDIR) == S_IFDIR);
#else
    return S_ISDIR(st.st_mode);
#endif
}

std::string normalize(const std::string& path) {
    std::string Normalized = path;

#ifdef WIN32
    // converts all '\' to '/' (only on WIN32)
    size_t i, imax;

    for (i = 0, imax = Normalized.length(); i < imax; i++)
        if (Normalized[i] == '\\') Normalized[i] = '/';

#endif
    // Remove leading './'
    while (!Normalized.compare(0, 2, "./")) Normalized = Normalized.substr(2);

    // Collapse '//' to '/'
    std::string::size_type pos = 1;

    while (true) {
        pos = Normalized.find("//", pos);

        if (pos == std::string::npos) break;

        Normalized.erase(pos, 1);
    }

    // Collapse '/./' to '/'
    pos = 0;

    while (true) {
        pos = Normalized.find("/./", pos);

        if (pos == std::string::npos) break;

        Normalized.erase(pos, 2);
    }

    // Collapse '[^/]+/../' to '/'
    std::string::size_type start = Normalized.length();

    while (true) {
        pos = Normalized.rfind("/../", start);

        if (pos == std::string::npos) break;

        start = Normalized.rfind('/', pos - 1);

        if (start == std::string::npos) break;

        if (!Normalized.compare(start, 4, "/../")) continue;

        Normalized.erase(start, pos - start + 3);
        start = Normalized.length();
    }

    return Normalized;
}

bool isReadable(const std::string& path) {
    return (access(path.c_str(), 0x4) == 0);
}

bool isWritable(const std::string& path) {
    return (access(path.c_str(), 0x2) == 0);
}

std::string getDirName(const std::string& path) {
    if (path == "") return path;

#ifdef WIN32  // WIN32 also understands '/' as the k_separator.
    std::string::size_type end = path.find_last_of(k_separator + "/");
#else
    std::string::size_type end = path.find_last_of(k_separator);
#endif

    if (end == path.length() - 1) {
#ifdef WIN32  // WIN32 also understands '/' as the k_separator.
        end = path.find_last_of(k_separator + "/", end);
#else
        end = path.find_last_of(k_separator, end);
#endif
    }

    if (end == std::string::npos) return "";

    return path.substr(0, end);
}

bool createDir(const std::string& dir, const std::string& parent = "") {
    std::string Dir;

    if (parent != "") Dir = parent + k_separator;

    Dir += dir;

    // Check whether the directory already exists and is writable.
    if (isDir(Dir) && isWritable(Dir)) return true;

    // Check whether the parent directory exists and is writable.
    if (!parent.empty() && (!isDir(parent) || !isWritable(parent)))
        return false;

    Dir = normalize(Dir);

    // ensure we have parent
    std::string actualParent = getDirName(Dir);

    // recursive create parent directory
    if (!actualParent.empty() && (!exist(actualParent)))
        createDir(actualParent);

#ifdef WIN32
    return (mkdir(Dir.c_str()) == 0);
#else
    return (mkdir(Dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == 0);
#endif
}

int main() {
    unzFile zfile = unzOpen64(epub_path);

    if (zfile == NULL) {
        printf("Can't open the epub file.\n");
    }

    int res = unzGoToFirstFile(zfile);

    if (res == UNZ_OK) {
        do {
            char file_name[max_path_length] = {0};
            unz_file_info64 file_info;
            unzGetCurrentFileInfo64(zfile, &file_info, file_name,
                                    max_path_length, NULL, 0, NULL, 0);

            char file_path[512] = {0};
            std::strcat(file_path, output_directory);
            std::strcat(file_path, file_name);

            createDir(getDirName(file_path));

            // must open current zfile first before unzReadCurrentFile
            if (unzOpenCurrentFile(zfile) != UNZ_OK) {
                unzClose(zfile);
                printf("Can't open current zfile %s", file_name);
            }

            std::FILE* fp = fopen(file_path, "w+");
            if (fp == nullptr) {
                printf("Can't open file %s\n", file_path);
            }

            char buf[buffer_length] = {0};
            int read = 0;

            while ((read = unzReadCurrentFile(zfile, buf, buffer_length)) > 0) {
                fwrite(buf, read, 1, fp);
            }

            if (read < 0) {
                unzCloseCurrentFile(zfile);
                unzClose(zfile);
                printf("The error occur in reading unzReadCurrentFile. %d\n",
                       read);
            }

            fclose(fp);

        } while ((res = unzGoToNextFile(zfile)) == UNZ_OK);
    }

    return 0;
}
