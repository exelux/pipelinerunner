cmake_minimum_required(VERSION 3.19)
project(pipelinerunner LANGUAGES CXX)

# ----------
# Ccache

include(cmake/utils.cmake)
enable_ccache()

# ----------
# Platform

find_platform(2.9.0)
message("Platform path: ${Platform_CONFIG}")
message("Platform version: ${PLT_VERSION}")

# ----------
# Complier options

include(cmake/set_compiler_options.cmake)
set_compiler_options()

# ----------
# Build dirs

set(PIPELINERUNNER_BUILD_DIR         "${CMAKE_BINARY_DIR}/bin")
set(PIPELINERUNNER_BIN_BUILD_DIR     "${PIPELINERUNNER_BUILD_DIR}")
set(PIPELINERUNNER_LIB_BUILD_DIR     "${PIPELINERUNNER_BUILD_DIR}/lib")
set(PIPELINERUNNER_PLUGINS_BUILD_DIR "${PIPELINERUNNER_BUILD_DIR}/plugins")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PIPELINERUNNER_BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PIPELINERUNNER_BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PIPELINERUNNER_BUILD_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# ----------
# Subprojects

add_subdirectory(src/PipelineRunner)
add_subdirectory(src/PipelineRunnerCommon)
add_subdirectory(src/PipelineRunnerPlugins)
add_subdirectory(pipelineutils)

# ----------
# Depends

libpack_package_name(QT_LIBPACK_PACKAGE_NAME)
deb_package_version(QT_LIBPACK_PACKAGE_VERSION ${QT_LIBPACK_PACKAGE_NAME})

plt_set_dependency(GLOBAL
    DEPENDENCY ${QT_LIBPACK_PACKAGE_NAME}
    VERSION_EQUAL_GREATER ${QT_LIBPACK_PACKAGE_VERSION}
)

plt_set_dependency(GLOBAL
    DEPENDENCY ${PLT_DEB_PACKAGE_NAME}
    VERSION_LESS  ${PLATFORM_MAX_VERSION}
    VERSION_EQUAL_GREATER ${PLATFORM_MIN_VERSION}
)

# ----------
# Deploy

if(UNIX)
    include(cicd/linux/deploy_deb.cmake)
endif()
