project(llhttp)

include_directories(include)
include_directories(helper)

aux_source_directory(src/ LLHTTP_SRC_FILES)
aux_source_directory(helper/ LLHTTP_SRC_FILES)

add_library(${PROJECT_NAME} STATIC ${LLHTTP_SRC_FILES})        
#set_target_properties (${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX _d)