#include "iconv.h"
#include <stdio.h>
#include <fstream>

int main(int argc, char **argv) {
    iconv_t cd = iconv_open("UTF-8", "GB18030");
    char dest[4096 + 4096];
    char buf[4096];
    char *bufptr = buf;
    char *destptr = dest;
    size_t buf_size = sizeof(buf);
    size_t dest_size = sizeof(dest);

    std::fstream fin;
    fin.open(argv[1], std::fstream::in);

    std::fstream fout;
    fout.open(argv[2], std::fstream::out);

    int cnt = 4;
    while (fin.read(buf, 4096)) {
        size_t in_size = fin.gcount();
        char *inptr = buf;
        char *outptr = dest;
        size_t out_size = dest_size;
        size_t res = iconv(cd, &inptr, &in_size, &outptr, &out_size);
        if (cnt > 0) {
            printf("in_size = %d, out_size = %d\n", in_size, out_size);
            printf("inptr offset = %d, outptr offset = %d\n", inptr - buf,
                   outptr - dest);
            printf("res = %d, errno = %d\n\n", res, errno);
            cnt--;
        }
        fout.write(dest, outptr - dest);
    }
    return 0;
}