cmake_minimum_required(VERSION 3.0.1)
project(mobi)

# CACHE INTERNAL "" in set 
set(MOBI_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

find_package(ZLIB)
find_package(LibXml2)

# parse the ncx and opf
add_definitions(-DUSE_XMLWRITER -DUSE_LIBXML2)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} MOBI_SOURSES)
add_library(${PROJECT_NAME} STATIC ${MOBI_SOURSES})
# the find_package(LibXml2) include 
# `set_target_properties(LibXml2::LibXml2 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${LIBXML2_INCLUDE_DIRS}")`
# "${LIBXML2_INCLUDE_DIRS}" is /usr/include/libxml2
# When target dependencies are specified using target_link_libraries(), 
# CMake will read the INTERFACE_INCLUDE_DIRECTORIES property from all target dependencies 
# to determine the build properties of the consumer. so don't need to include libxml2's manually
target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB LibXml2::LibXml2)
target_include_directories(${PROJECT_NAME} PUBLIC ${MOBI_INCLUDE_DIRS})
