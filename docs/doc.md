# ebook-web

使用浏览器阅读epub和mobi

## 功能

- [x] epub的解析与阅读
- [x] mobi和azw3的解析与阅读
- [ ] txt解析与阅读
- [x] 生成目录跳转
- [x] http解析，后端传输
- [ ] 前端书架管理

## 3rd-party

- minizip
- zlib
- asio
- llhttp
- spdlog
- cxxopts
- nlohmann/json
- xml2json
- inja
- libmobi
- sqlite3
- libiconv
- compact_enc_det


## 思路

1. epub就是将xhtml和css打包后用zip压缩，所以需要使用minizip对其进行解压
2. epub的目录信息在toc.ncx下，是一个xml，所以需要一个xml解析器来生成目录
3. 后端使用之前用asio构建的Jhttp_server
4. 前端找一个框架，能够显示电子书的封面就行
5. 利用cxxopts构建命令行
6. 需要模板引擎来对每本书的目录生成对应的html，找了一下只有inja比较好，但它是依赖json的，所以需要将xml解析再生成json，隔了一层性能差了了一些，不过对于c++来说无所谓了
7. 云端管理书架，能够上传下载

## 遇到的问题

### xml生成目录

epub的目录在`toc.ncx`文件下，xml的解析不是什么问题，主要的问题在于模板引擎，c++的模板引擎特别少，找了一圈只有一个inja库还可以，但是依赖于nlohmann/json，也就是数据结构必须是json才行，其他的库基本都依赖于boost，我也是实在不想用boost，所以只能用inja中间套一层

但是nlohmann/json的生成感觉很不好用，或者是我还不熟悉，所以xml转json我暂时还写不出来，现在是利用github上一个xml转json的库来进行，但是这个库依赖于rapidjson和rapidxml，所以又得封装一层，而且rapidxml太老了，是不支持宽字符的，所以这块是需要重写的，现在的基本功能是完成了

### epub文件处理

epub的结构就是html+zip，所以使用minizip将其解压到临时文件夹即可，比如`tmp/`下，这样也就是普通后端处理

Q: minizip中没有直接解压到本地的接口，所以需要自己进行对每个entry进行读取封装，但是对于在本地创建文件时需要创建目录，这里有些麻烦，fopen等是不能创建目录的
A: github上有个对minizip的封装库zipper，不过我只需要其中的解压到本地功能，所以自己写一个算了，C++17中有`<filesystem>`可以使用`create_directory`创建目录

### 模板渲染后的数据如何写入response

connection中response与模板渲染后的数据如何关联起来，现在是在connection中的handleRequest中将response中的内容填充进去，耦合太强了，需要加入中间层，而且内容应该共用。并且应该对于模板进行选择操作，即可选是否载入模板

### One Definition Rule

期间遇到了ODR问题，xml2json这个库违反了这一原则

### 暂时先做一个cli工具

本来想的是做一个类似云书库，但是一个最大的问题是写的耦合度太高了，用C++写网络框架，connection中如何将content抽象出来写回，这个地方有点复杂，需要研究一下回调怎么写，目前只能先做成cli形式

### 异步写回的数据生命周期

response中必须使用全局变量，如果使用栈分配的临时变量则客户端无法收到消息，会报错`net::invalid_response_http_header`

```cpp
static const std::string response_status = "HTTP/1.1 200 OK\r\n";
buffers.push_back(boost::asio::buffer(response_status));
```

这是由于异步的特性，`boost::asio::buffer`只是作为一个标记的缓存，本身不对数据进行存储，因此当异步写回时，栈分配的临时变量已经被释放，因此会出现错误。可以使用`boost::asio::streambuf`，它是类似于流传输，保存了传输数据，因此可以确保异步写回时的数据生命周期

### mobi结构

可以先看看下面这个文档：
https://wiki.mobileread.com/wiki/MOBI#MOBI_Header

总体来说，mobi中的信息以链表形式存储，链表信息存储在mobi文件开头的固定字节处

### txt的切分章节

这块比较难搞，有以下几个问题：

1. 切分章节，可以通过正则表达式来划分，划分成功则将其存储至单章文件
2. 考虑使用数据库将每章的内容存入sqlite3中
3. c++中正则对unicode支持很差，只能支持到utf-8，所以要么引入icu库进行统一换算，要么手动对unicode转utf-8

编码是一个问题，起码要考虑以下三种编码：

1. GB18030，是国内自己的编码标准，不带BOM，两个字节表示一个字，一般下载到的国内小说都是个格式
2. UTF-8，不带BOM，三个字节表示一个字，部分网络小说是这个格式
3. ANSI, 这种一般是英文的文本，暂时不考虑，因为我分章节的正则也没考虑英文的情况

如何判断是哪种编码是比较头疼的问题，起码需要区分开GB18030和UTF-8，找了一下有两个库可以用：

- google有一个compact_enc_det库，速度快，对于短文本识别的情况可能较好，应用在Chromium里
- ICU有Character Set Detection模块，速度可能较CED慢

测试了一下ICU内的`ucsdet_detect`准确率还是可以的

Q: 遇到了边界切分的问题，流读入固定字节的字符时，最后读入的几个字节不一定是一个完整的字符，这是字符边界切分问题
A: ICU中`ucnv_toUnicode`函数中可以通过偏移量`offset`计算出边界，但是很麻烦，而且一个问题是ICU默认转换为UTF-16，想转化为UTF-8还得自己手动再转换一边

https://www.ibm.com/developerworks/cn/opensource/os-cn-icu4c-ls1/index.html

最后还是采用libiconv+CED的方式

libiconv中`size_t res = iconv(cd, &inptr, &leftsize, &outptr, &outsize);`，好就好在会主动处理边界问题，如果出现跨边界字符，会先不将处理并抛出`errno == EINVAL`，所以需要对边界问题单独处理，基本思路是将剩下的字符复制到下一次读取的字符串的开头

### html无法直接读取txt

原来单纯用html是无法打开本地文件的，想了想两种解决方式：

1. 利用html+javascript，找了一下网上的方案，有一种可以使用html的FileReader，但是目前找到的只能使用打开文件按钮来显示打开的文件信息，不知道单纯的点击元素是否能达到该效果
2. txt转html，这个应该是可以做，但是不知道速度如何，而且说实话这么处理感觉很没必要

因为安全原因，纯靠前端是无法直接打开本地文件的，但是因为ebook-web本身实现了后端，是通过http协议传输字节流的，所以不需要考虑这个问题，直接在后端中打开txt文件并将文本流传输给web即可




