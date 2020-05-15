
# aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src JEPUB_SERVER_SRC)
set(JEPUB_SERVER_SRC
    connection_manager.cpp
    connection.cpp
    main.cpp
    response.cpp
    server.cpp
    zipper.cpp
    templates.cpp
)

# add_library(${PROJECT_NAME} STATIC ${JEPUB_SERVER_SRC})
add_executable(${PROJECT_NAME} ${JEPUB_SERVER_SRC})
target_link_libraries(${PROJECT_NAME}
    pthread
    ${LLHTTP_LIBRARIES} 
    asio
    nlohmann_json 
    inja 
    spdlog 
    xml2json
    ${MINIZIP_LIBRARIES}
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include ${MINIZIP_INCLUDE_DIRS})
