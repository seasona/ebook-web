#include "llhttp/include/llhttp.h"
#include <cstdio>
#include <cstring>

static const char request[] =
    "POST http://www.baidu.com/java/web?flag=1#function HTTP/1.1\r\n"
    "Host: github.com\r\n"
    "DNT: 1\r\n"
    "Accept-Encoding: gzip, deflate, sdch\r\n"
    "Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4\r\n"
    "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1) "
    "AppleWebKit/537.36 (KHTML, like Gecko) "
    "Chrome/39.0.2171.65 Safari/537.36\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,"
    "image/webp,*/*;q=0.8\r\n"
    "Referer: https://github.com/joyent/http-parser\r\n"
    "Connection: keep-alive\r\n"
    "Transfer-Encoding: chunked\r\n"
    "Cache-Control: max-age=0\r\n\r\nb\r\nhello world\r\n0\r\n";

int printBuffer(llhttp_t* parser, const char* at, size_t length) {
    for (int i = 0; i < length; i++) {
        printf("%c", *(at + i));
    }
    printf("\n\n");
    return 0;
}

int main() {
    llhttp_t parser;
    llhttp_settings_t setting;

    llhttp_settings_init(&setting);

    setting.on_url = printBuffer;
    setting.on_header_field = printBuffer;
    setting.on_header_value = printBuffer;
    setting.on_body = printBuffer;

    llhttp_init(&parser, HTTP_BOTH, &setting);
    int request_len = strlen(request);

    llhttp_errno err = llhttp_execute(&parser, request, request_len);

    if (err == HPE_OK) {
        printf("http parse successful!\n");
    } else {
        fprintf(stderr, "Parse error: %s %s\n", llhttp_errno_name(err),
                parser.reason);
    }

    return 0;
}
