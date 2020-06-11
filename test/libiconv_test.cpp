#include "iconv.h"
#include <cstdio>
#include <fstream>
#include <iostream>

int main(int argc, char **argv) {
    iconv_t cd = iconv_open("UTF-8", "GB18030");
    char dest[4096 + 4096];
    char buf[4096 + 4096];
    char *bufptr = buf;
    char *destptr = dest;
    size_t buf_size = sizeof(buf);
    size_t dest_size = sizeof(dest);

    std::fstream fin;
    fin.open(argv[1], std::fstream::in);

    std::fstream fout;
    fout.open(argv[2], std::fstream::out);

    // int cnt = 10;
    int inbufrest = 0;
    while (fin.read(buf + 4096, 4096).gcount()>0) {
        char *lastptr = buf + 4096 - inbufrest;
        char *inptr = lastptr;
        size_t leftsize = inbufrest + fin.gcount();
        std::cout<<fin.gcount()<<std::endl;
        char *outptr = dest;
        size_t outsize = dest_size;
        size_t res = iconv(cd, &inptr, &leftsize, &outptr, &outsize);
        // std::cout << inptr-lastptr << "--" << inbufrest << std::endl;
        // if (cnt > 0) {
        //     printf("leftsize = %d, outsize = %d\n", leftsize, outsize);
        //     printf("inptr offset = %d, outptr offset = %d\n", inptr -
        //     lastptr,
        //            outptr - dest);
        //     printf("res = %d, errno = %d\n\n", res, errno);
        //     // cnt--;
        // }
        if (errno == EINVAL) {
            inbufrest = leftsize;
            if (leftsize > 0) {
                /* Like memcpy(inbuf+4096-insize,inptr,insize), except that
                    we cannot use memcpy here, because source and destination
                    regions may overlap. */
                // move left character to the front of next operation
                char *restptr = buf + 4096 - leftsize;
                do {
                    *restptr++ = *inptr++;
                } while (--leftsize > 0);
            }
        }
        fout.write(dest, outptr - dest);
    }
    return 0;
}