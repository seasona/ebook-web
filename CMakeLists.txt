cmake_minimum_required(VERSION 3.9)

project(Jhttp_server LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE
        Release
        CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

add_subdirectory(extern)
add_subdirectory(src)
add_subdirectory(test)

# unit test
option(JHTTP_SERVER_TEST OFF)

if(JHTTP_SERVER_TEST)
    add_subdirectory(test)
endif()
