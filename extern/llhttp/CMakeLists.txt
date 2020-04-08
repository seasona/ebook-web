project(llhttp)

set(LLHTTP_LIBRARIES ${PROJECT_NAME} CACHE INTERNAL "" )
set(LLHTTP_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/helper)

aux_source_directory(src/ LLHTTP_SRC_FILES)
aux_source_directory(helper/ LLHTTP_SRC_FILES)

add_library(${PROJECT_NAME} STATIC ${LLHTTP_SRC_FILES} )        
target_include_directories(${PROJECT_NAME} PUBLIC ${LLHTTP_INCLUDE_DIRS})
