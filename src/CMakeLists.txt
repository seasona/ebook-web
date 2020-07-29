
# aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src JEBOOK_SERVER_SRC)
set(JEBOOK_SERVER_SRC
    connection_manager.cpp
    connection.cpp
    ebook.cpp
    main.cpp
    response.cpp
    server.cpp
    zipper.cpp
    templates.cpp
    epub_book.cpp
    mobi_book.cpp
    txt_book.cpp
    util.cpp
)


# add_library(${PROJECT_NAME} STATIC ${JEBOOK_SERVER_SRC})
add_executable(${PROJECT_NAME} ${JEBOOK_SERVER_SRC})
target_link_libraries(${PROJECT_NAME}
    pthread
    ${LLHTTP_LIBRARIES} 
    asio
    nlohmann_json 
    inja 
    spdlog 
    xml2json
    ${MINIZIP_LIBRARIES}
    mobi
    ced
    cxxopts
)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include 
    ${MINIZIP_INCLUDE_DIRS} 
)

