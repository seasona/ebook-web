cmake_minimum_required(VERSION 3.10)

project(Jebook_server LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE
        Release
        CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_MODULE_PATH {CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(USE_SYSTEM_LIBS ON)

if(USE_SYSTEM_LIBS)
    message(STATUS "Trying to use System Libraries...")
    find_package(ZLIB)
    find_package(MiniZip)
endif(USE_SYSTEM_LIBS)

add_subdirectory(third_party)
add_subdirectory(src)
add_subdirectory(test)

# unit test
option(JEBOOK_SERVER_TEST OFF)

if(JEBOOK_SERVER_TEST)
    add_subdirectory(test)
endif()
