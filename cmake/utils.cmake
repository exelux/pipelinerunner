function(enable_ccache)
    find_program(CCACHE_FOUND ccache)
    
    if(NOT CCACHE_FOUND)
        message(WARNING "ccache not found, install ccache to accelerate rebuild")
        return()
    endif()

    message(STATUS "ccache found, using it for build")

    if (WIN32)
        string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_DEBUG            "${CMAKE_C_FLAGS_DEBUG}")
        string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG}")
        string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_RELWITHDEBINFO   "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
        string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    endif()

    set(CMAKE_C_LINKER_LAUNCHER     ${CCACHE_FOUND} PARENT_SCOPE)
    set(CMAKE_C_COMPILER_LAUNCHER   ${CCACHE_FOUND} PARENT_SCOPE)
    set(CMAKE_CXX_LINKER_LAUNCHER   ${CCACHE_FOUND} PARENT_SCOPE)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_FOUND} PARENT_SCOPE)
endfunction()


macro(find_platform MIN_VERSION)
    if(NOT ${MIN_VERSION} MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+$")
        message(FATAL_ERROR "Unsupported version format: '${VERSION_STRING}'. Expected X.Y.Z")
    endif()

    string(REPLACE "." ";" VERSION_LIST ${MIN_VERSION})

    list(GET VERSION_LIST 0 MAJOR)
    list(GET VERSION_LIST 1 MINOR)
    list(GET VERSION_LIST 2 PATCH)

    math(EXPR MAX_MINOR "${MINOR} + 1")

    set(PLATFORM_MIN_VERSION ${MIN_VERSION})
    set(PLATFORM_MAX_VERSION ${MAJOR}.${MAX_MINOR})

    find_package(Platform ${PLATFORM_MIN_VERSION}...<${PLATFORM_MAX_VERSION} QUIET)
    if(NOT Platform_FOUND)
        if(WIN32)
            list(APPEND CMAKE_PREFIX_PATH "C:/Platform")
        elseif(UNIX)
            list(APPEND CMAKE_PREFIX_PATH "/opt/platform")
        endif()

        message("Platform package not found. Trying default path.")
        find_package(Platform ${PLATFORM_MIN_VERSION}...<${PLATFORM_MAX_VERSION} REQUIRED)
    endif()
endmacro()


function(get_all_targets DIR RESULT_VAR)
    get_property(TGTS DIRECTORY "${DIR}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(TGT IN LISTS TGTS)
        list(APPEND ${RESULT_VAR} ${TGT})
    endforeach()

    get_property(SUBDIRS DIRECTORY "${DIR}" PROPERTY SUBDIRECTORIES)
    foreach(SUBDIR IN LISTS SUBDIRS)
        get_all_targets("${SUBDIR}" ${RESULT_VAR})
    endforeach()

    set(${RESULT_VAR} "${${RESULT_VAR}}" PARENT_SCOPE)
endfunction()


function(libpack_package_name RESULT)
    plt_libpack_package_name(QT_LIBPACK_PACKAGE_NAME)

    if(QT_LIBPACK_PACKAGE_NAME STREQUAL "LIBPACK-NOTFOUND")
      message(FATAL_ERROR "qt-libpack package name not found")
    endif()

    set(${RESULT} ${QT_LIBPACK_PACKAGE_NAME} PARENT_SCOPE)
endfunction()


function(deb_package_version RESULT PACKAGE)
    plt_deb_package_version(${PACKAGE} VERSION)

    if(VERSION STREQUAL "VERSION-NOTFOUND")
      message(FATAL_ERROR "${PACKAGE} package version not found")
    endif()

    set(${RESULT} ${VERSION} PARENT_SCOPE)
endfunction()
