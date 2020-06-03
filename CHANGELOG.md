# Changelog

## unreleased

- add cxxopts
- support read epub2
- reduce the degree of coupling
- implement own xml to json, not use xml2json
- support read txt
- transform async + multithread

## 0.3.0 - 2020/5/28

### Added

- support read mobi and azw formart ebook

## 0.2.0 - 2020/5/16

### Changed

- the server now support read epub3 book
- Changed the handleReqeust function in Connection

### Added

- add the inja as the template library and wrappered it, it depend on nlohmann::json
- add xml2json to convert xml to json
- add minizip and wrapper it to unzip the epub

## 0.1.1 - 2020/3/1

### Changed

- use asio instead of boost::asio standalone

### fixed

- fix css problem in index.html

## 0.1.0 - 2020/2/29

### Added

- add the basic function of http server: establish connection, http request parse, consititue response and async read&write
- add the module llhttp to parse http request
- add the module boost::asio as the aysnc framework
- add the module spdlog as the output log


