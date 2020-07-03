cmake_minimum_required(VERSION 3.10)

project(Jebook_server LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE
        Release
        CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_MODULE_PATH {CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


include(FetchContent) # 引入该CMake模块
FetchContent_Declare( # 声明依赖的相关信息
  cxxopts
  GIT_REPOSITORY git@github.com:jarro2783/cxxopts.git
  GIT_TAG        v2.2.0
  GIT_SHALLOW    TRUE # 不拉取完整历史，相当于`git clone --depth=1`
)

FetchContent_GetProperties(cxxopts)
if(NOT cxxopts_POPULATED) # 确保只拉取一次
  FetchContent_Populate(cxxopts) # 此函数执行后将设置AAA_POPULATED变量
  # 通过AAA_SOURCE_DIR就可以拿到源码所在目录的路径了，除此外还有其他变量可以用，见CMake文档
  message(STATUS "cxxopts source dir: ${cxxopts_SOURCE_DIR}")
  add_subdirectory(${cxxopts_SOURCE_DIR})
endif ()

option(USE_SYSTEM_LIBS ON)

if(USE_SYSTEM_LIBS)
    message(STATUS "Trying to use System Libraries...")
    find_package(ZLIB)
    find_package(MiniZip)
endif(USE_SYSTEM_LIBS)

add_subdirectory(third_party)
add_subdirectory(src)
# add_subdirectory(test)

# unit test
option(JEBOOK_SERVER_TEST OFF)

if(JEBOOK_SERVER_TEST)
    add_subdirectory(test)
endif()
