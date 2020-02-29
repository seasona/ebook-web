cmake_minimum_required(VERSION 3.9)

project(Jhttp_server LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

include_directories(.)
include_directories(include)
include_directories(asio/include)
include_directories(spdlog)
include_directories(llhttp/include)
include_directories(llhttp/helper)

add_subdirectory(llhttp)

link_libraries(llhttp)
link_libraries(pthread)

# unit test
option(JHTTP_SERVER_TEST OFF)

if(JHTTP_SERVER_TEST)
    add_subdirectory(test)
endif()

aux_source_directory(src/ JHTTP_SERVER_SRC)

add_executable(Jhttp_server ${JHTTP_SERVER_SRC})


