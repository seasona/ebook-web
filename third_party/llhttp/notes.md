# http_parser

## http报头结构

```c
static const char data[] =
    "POST /joyent/http-parser HTTP/1.1\r\n"
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
```

http的结构如下图所示:

request:

![](https://images2017.cnblogs.com/blog/1090126/201711/1090126-20171115074856609-255880765.png)

response:

![](https://images2017.cnblogs.com/blog/1090126/201711/1090126-20171115074903718-2136115327.png)

## parser流程

http_parser的原理就是一个有限的状态机，对传入的http头进行解析，整个的流程是这样的：

```c
struct http_parser_settings {
  http_cb      on_message_begin;
  http_data_cb on_url;      // return url
  http_data_cb on_status;   // return status
  http_data_cb on_header_field; // return field of header
  http_data_cb on_header_value; // return value of header
  http_cb      on_headers_complete;
  http_data_cb on_body;         // return http bodys
  http_cb      on_message_complete;
  /* When on_chunk_header is called, the current chunk length is stored
   * in parser->content_length.
   */
  http_cb      on_chunk_header;
  http_cb      on_chunk_complete;
};

struct http_parser {
  /** PRIVATE **/
  unsigned int type : 2;         /* enum http_parser_type */
  unsigned int state : 7;        /* enum state from http_parser.c */
  unsigned int header_state : 7; /* enum header_state from http_parser.c */
  unsigned int index : 7;        /* index into current matcher */
  unsigned int lenient_http_headers : 1;
  unsigned int flags : 16;       /* F_* values from 'flags' enum; semi-public */

  uint32_t nread;          /* # bytes read in various scenarios */
  uint64_t content_length; /* # bytes in body (0 if no Content-Length header) */

  /** READ-ONLY **/
  unsigned short http_major;
  unsigned short http_minor;
  unsigned int status_code : 16; /* responses only */
  unsigned int method : 8;       /* requests only */
  unsigned int http_errno : 7;

  /* 1 = Upgrade header was present and the parser has exited because of that.
   * 0 = No upgrade header present.
   * Should be checked when http_parser_execute() returns in addition to
   * error checking.
   */
  unsigned int upgrade : 1;

  /** PUBLIC **/
  void *data; /* A pointer to get hook to the "connection" or "socket" object */
};

/* Executes the parser. Returns number of parsed bytes. Sets
 * `parser->http_errno` on error. */
size_t http_parser_execute(http_parser *parser,
                           const http_parser_settings *settings,
                           const char *data,
                           size_t len);
```

先设立`http_parser_settings`中的回调函数，这些回调函数在相关信息被提取后调用，比如`on_url`就可以设立为以下的函数将url打印出来:

```c
static int on_data_url(http_parser* p, const char *at, size_t length) {
  printf("=========url==========\n");
  for(int i=0;i<length;i++){
    printf("%c", *(at+i));
  }
  printf("\n");
  return 0;
}
```

因为http解析的过程实际就类似于状态机，所以是按照http报头的结构顺序进行解析并且调用回调函数的

然后通过`http_parser_execute`进行解析，这里其实只将`method`和`http_version`存储在`struct http_parser`中，如`url`,`header_field`,`header_value`,`body`等，都是通过回调函数传递出来的，所以需要在回调函数中将其存储下来。

## url parse

传统形式的URL格式： schema://host:port/path?query#fragment

- schema: 协议，例如http , https,  ftp等
- host: 域名或者ip地址
- port： 端口，http默认端口80，可以省略
- path： 路径，例如/abc /a/b/c
- query: 查询参数，例如 token=sdfs2223fds2&name=sdffaf
- fragment: 锚点（哈希Hash），用于定位页面的某个位置。

符合规则的URL: http://www.baidu.com/java/web?flag=1#function

http_parser中的url解析不包括url的编码与解码