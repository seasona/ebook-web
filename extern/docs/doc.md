# epub-web

使用浏览器阅读epub

## 功能

1. epub的解压，目录生成
2. 后端传输
3. 前端书架管理

## 3rd-party

- minizip
- zlib
- asio
- llhttp
- spdlog
- pugixml
- cxxopts
- nlohmann/json
- inja

## 思路

1. epub就是将xhtml和css打包后用zip压缩，所以需要使用minizip对其进行解压
2. epub的目录信息在toc.ncx下，是一个xml，所以需要一个xml解析器来生成目录
3. 后端使用之前用asio构建的Jhttp_server
4. 前端找一个框架，能够显示电子书的封面就行
5. 利用cxxopts构建命令行
6. 需要模板引擎来对每本书的目录生成对应的html，找了一下只有inja比较好，但它是依赖json的，所以需要将xml解析再生成json，隔了一层性能差了了一些，不过对于c++来说无所谓了
7. 云端管理书架，能够上传下载

### xml生成目录

epub的目录在`toc.ncx`文件下，xml的解析不是什么问题，主要的问题在于模板引擎，c++的模板引擎特别少，找了一圈只有一个inja库还可以，但是依赖于nlohmann/json，也就是数据结构必须是json才行，其他的库基本都依赖于boost，我也是实在不想用boost，所以只能用inja中间套一层

但是nlohmann/json的生成感觉很不好用，或者是我还不熟悉，所以xml转json我暂时还写不出来，现在是利用github上一个xml转json的库来进行，但是这个库依赖于rapidjson和rapidxml，所以又得封装一层，而且rapidxml太老了，是不支持宽字符的，所以这块是需要重写的，现在的基本功能是完成了

## 结构


