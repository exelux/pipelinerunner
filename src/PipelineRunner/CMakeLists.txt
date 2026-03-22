cmake_minimum_required(VERSION 3.14)
project(pipelinerunner LANGUAGES CXX)

# ----------
# Complier options

set_compiler_options()

# ----------
# Sources

set(PROJECT_HEADERS
    pipelinerunner.h
)

set(PROJECT_SOURCES
    main.cpp
    pipelinerunner.cpp
)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

# ----------
# Dependencies

plt_link_qt_libraries(${PROJECT_NAME} PRIVATE COMPONENTS
    Core
)

plt_link_plt_libraries(${PROJECT_NAME} PRIVATE COMPONENTS
    Core
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    PipelineRunnerCommon
)

# ----------
# Build directory

if (DEFINED PIPELINERUNNER_BIN_BUILD_DIR)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${PIPELINERUNNER_BIN_BUILD_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${PIPELINERUNNER_BIN_BUILD_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${PIPELINERUNNER_BIN_BUILD_DIR}
    )
endif()
