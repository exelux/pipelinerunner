cmake_minimum_required(VERSION 3.19)
project(PipelineRunnerCommon LANGUAGES CXX)

# ----------
# Complier options

set_compiler_options()

# ----------
# Sources

set(PROJECT_HEADERS
    ${PROJECT_NAME}_global.h

    include/Common/Services/iconfigservice.h
    include/Common/Services/iconverterservice.h
    include/Common/Services/ijobhandle.h
    include/Common/Services/irabbitservice.h
    include/Common/Services/irunnerservice.h
    include/Common/Services/loadorder.h

    include/Common/Structs/config.h
    include/Common/Structs/jobinfo.h
    include/Common/Structs/message.h
)

set(PROJECT_SOURCES
    src/Services/iconfigservice.cpp
    src/Services/iconverterservice.cpp
    src/Services/ijobhandle.cpp
    src/Services/irabbitservice.cpp
    src/Services/irunnerservice.cpp

    src/Structs/jobinfo.cpp
    src/Structs/message.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${PROJECT_HEADERS}
    ${PROJECT_SOURCES}
)

# ----------
# Dependencies

plt_link_qt_libraries(${PROJECT_NAME} PRIVATE COMPONENTS
    Core
)

plt_link_plt_libraries(${PROJECT_NAME} PRIVATE COMPONENTS
    Core
    Network
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    toolkit
)

# ----------
# Build properties

if (PIPELINERUNNER_LIB_BUILD_DIR)
    set_target_properties(${PROJECT_NAME}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${PIPELINERUNNER_LIB_BUILD_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${PIPELINERUNNER_LIB_BUILD_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${PIPELINERUNNER_LIB_BUILD_DIR}
    )
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE COMMON_LIBRARY)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
