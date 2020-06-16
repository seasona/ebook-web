# add_executable(llhttp_test llhttp_test.cpp)

# add_executable(spdlog_test spdlog_test.cpp)

# add_executable(inja_test inja_test.cpp)
# target_link_libraries(inja_test inja xml2json)

# add_executable(minizip_test minizip_test.cpp)
# target_include_directories(minizip_test PRIVATE ${MINIZIP_INCLUDE_DIRS})
# target_link_libraries(minizip_test ${MINIZIP_LIBRARIES})

# add_executable(zipper_test zipper_test.cpp)
# target_include_directories(zipper_test PRIVATE ${CMAKE_SOURCE_DIR}/include)
# target_link_libraries(zipper_test Jebook_server)

add_executable(ced_test ced_test.cpp)
target_link_libraries(ced_test ced)
