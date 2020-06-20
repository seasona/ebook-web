# ebook-web

[![Build Status](https://travis-ci.com/seasona/ebook-web.svg?branch=master)](https://travis-ci.com/seasona/ebook-web)
[![License](https://img.shields.io/github/license/seasona/ebook-web)](https://img.shields.io/github/license/seasona/ebook-web)

ebook-web is a CLI tool to read ebook of epub, mobi, azw3 and txt format in your browser

## Table of Contents

* [About the Project](#about-the-project)
  * [Feature](#feature)
  * [Platform](#platform)
  * [Built With](#built-with)
* [Getting Started](#getting-started)
  * [Prerequisites](#prerequisites)
  * [Installation](#installation)
* [Usage](#usage)
* [License](#license)


## About The Project

### Feature

- [x] parse and read epub format ebook
- [x] parse and read mobi, azw3 format ebook
- [x] parse and read txt format ebook
- [x] support create charpter directory of all format ebook
- [ ] maintain your local ebook shelf

### Platform

linux

### Built With

g++ 7.5.0 and higher

cmake 3.10.2 and higher

## Getting Started

### Prerequisites

ebook-web is depend on zlib, minizip and libxml2, you can choose to build them through sourse code from the ebook-web or use the system lib installed locally

```shell
sudo apt-get install -y zlib1g-dev libxml2-dev libminizip-dev
```

### Installation

```shell
cd ebook-web/
mkdir -p build && cd build
cmake ../ -DUSE_SYSTEM_LIBS=1  
make 
```

## Usage

```shell
./ebook-web <address> <port> <ebook_path> <out_directory>
```

## License

Distributed under the MIT License. See `LICENSE` for more information.


