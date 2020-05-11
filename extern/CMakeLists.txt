
add_subdirectory(llhttp)

## A primary use-case for INTERFACE libraries is header-only libraries.
## https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#interface-libraries

# spdlog header-only
add_library(spdlog INTERFACE)
target_include_directories(spdlog INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include)

# nlohmann/json header-only
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/json/include)

# inja header-only
add_library(inja INTERFACE)
target_include_directories(inja INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/inja)
target_link_libraries(inja INTERFACE nlohmann_json)

# asio header-only
add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/asio/include)

# xml2json 
add_library(xml2json INTERFACE)
target_include_directories(xml2json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/xml2json/include)
