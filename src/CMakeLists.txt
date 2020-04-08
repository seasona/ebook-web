
# aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src JHTTP_SERVER_SRC)
set(JHTTP_SERVER_SRC
    connection_manager.cpp
    connection.cpp
    main.cpp
    response.cpp
    server.cpp
)

add_executable(${PROJECT_NAME} ${JHTTP_SERVER_SRC})
target_link_libraries(${PROJECT_NAME} pthread ${LLHTTP_LIBRARIES} asio nlohmann_json inja spdlog)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)