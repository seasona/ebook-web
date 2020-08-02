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

find_package(GTest REQUIRED)

if(NOT GTEST_FOUND)
    message(STATUS "There is no gtest local, so fetch from github. Version: release-1.10.0")
    FetchContent_Declare( # 声明依赖的相关信息
        gtest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.10.0
        GIT_SHALLOW    TRUE # 不拉取完整历史，相当于`git clone --depth=1`
    )

    FetchContent_GetProperties(gtest)
    if(NOT gtest_POPULATED) # 确保只拉取一次
        FetchContent_Populate(gtest) # 此函数执行后将设置AAA_POPULATED变量
        # 通过AAA_SOURCE_DIR就可以拿到源码所在目录的路径了，除此外还有其他变量可以用，见CMake文档
        message(STATUS "gtest source dir: ${gtest_SOURCE_DIR}")
        add_subdirectory(${gtest_SOURCE_DIR})
    endif ()
endif()

set(UNIT_TEST_LIST 
    ced_test
)

foreach(src ${UNIT_TEST_LIST})
    add_executable(${src} ${src}.cpp)
    target_link_libraries(${src} ced GTest::GTest GTest::Main)
    add_test(${src} ${src})
endforeach()
