cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

# Project name
project(amqpcpp LANGUAGES CXX)
set (VERSION_MAJOR 4)
set (VERSION_MINOR 3)
set (VERSION_PATCH 26)
set (SO_VERSION ${VERSION_MAJOR}.${VERSION_MINOR})

# Build options
option(AMQP-CPP_LINUX_TCP "Build linux sockets implementation." On)

# Pass version number to source files as macro
add_compile_definitions(VERSION=${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

# Ensure c++11 on all compilers
set (CMAKE_CXX_STANDARD 14)

# Set include/ as include directory
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/include)

# macro that adds a list of provided source files to a list called SRCS.
# if variable SRCS does not yet exist, it is created.
macro (add_sources)
    file (RELATIVE_PATH _relPath "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    foreach (_src ${ARGN})
        if (_relPath)
            list (APPEND SRCS "${_relPath}/${_src}")
        else()
            list (APPEND SRCS "${_src}")
        endif()
    endforeach()
    if (_relPath)
        # propagate SRCS to parent directory
        set (SRCS ${SRCS} PARENT_SCOPE)
    endif()
endmacro()

# add source files
aux_source_directory(src src_MAIN)
if(AMQP-CPP_LINUX_TCP)
    aux_source_directory(src/linux_tcp src_LINUX_TCP)
endif()

#---------------------------------------------------------------------------------------------------
# Build
#---------------------------------------------------------------------------------------------------

# create shared lib
add_library(${PROJECT_NAME} SHARED ${src_MAIN} ${src_LINUX_TCP})

if(DEFINED RABBIT_CLIENT_BIN_BUILD_DIR)
    set_target_properties(${PROJECT_NAME}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE_UPPER} ${RABBIT_CLIENT_BIN_BUILD_DIR}
        LIBRARY_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE_UPPER} ${RABBIT_CLIENT_BIN_BUILD_DIR}
        RUNTIME_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE_UPPER} ${RABBIT_CLIENT_BIN_BUILD_DIR}
        )
endif()

#---------------------------------------------------------------------------------------------------
# Deploy
#---------------------------------------------------------------------------------------------------
if(DEFINED RABBIT_CLIENT_DEPLOY_DIR)
    install(TARGETS ${PROJECT_NAME}
            LIBRARY DESTINATION ${RABBIT_CLIENT_DEPLOY_DIR}
            )
endif()

#---------------------------------------------------------------------------------------------------
# Submodule support
# ------------------------------------------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
    )

if(AMQP-CPP_LINUX_TCP)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS} pthread)
    # Find OpenSSL and provide include dirs
    find_package(OpenSSL REQUIRED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${OPENSSL_INCLUDE_DIR})
endif()
