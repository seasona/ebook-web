
# add_executable(llhttp_test llhttp_test)

# add_executable(spdlog_test spdlog_test.cpp)

add_executable(inja_test inja_test.cpp)
target_link_libraries(inja_test inja xml2json)