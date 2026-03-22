cmake_minimum_required(VERSION 3.19)
project(RtrHpoRunnerConverterPlugin LANGUAGES CXX)

# ----------
# Complier options

set_compiler_options()

# ----------
# Sources

set(PROJECT_HEADERS
    Plugin/converterplugin.h

    Service/jobhandle.h
    Service/converterservice.h
)

set(PROJECT_SOURCES
    Plugin/converterplugin.cpp

    Service/jobhandle.cpp
    Service/converterservice.cpp
)

add_library(${PROJECT_NAME} MODULE
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
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    PipelineRunnerCommon
)

# ----------
# Build properties

if (PIPELINERUNNER_PLUGINS_BUILD_DIR)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${PIPELINERUNNER_PLUGINS_BUILD_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${PIPELINERUNNER_PLUGINS_BUILD_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${PIPELINERUNNER_PLUGINS_BUILD_DIR}
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
