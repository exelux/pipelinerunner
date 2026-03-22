cmake_minimum_required(VERSION 3.19)
project(RtrHpoRunnerRabbitPlugin LANGUAGES CXX)

# ----------
# Complier options

set_compiler_options()

# ----------
# Sources

set(PROJECT_HEADERS
    Plugin/rabbitplugin.h
    Service/rabbitservice.h
)

set(PROJECT_SOURCES
    Plugin/rabbitplugin.cpp
    Service/rabbitservice.cpp
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
    rabbitmq
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
