cmake_minimum_required(VERSION 3.19)
project(toolkit LANGUAGES CXX)

set(PROJECT_HEADERS
    include/toolkit/toolkit_global.h

    include/toolkit/expected.h
    include/toolkit/ipandport.h
    include/toolkit/jsonextractor.h
)

set(PROJECT_SOURCES
    src/ipandport.cpp
    src/jsonextractor.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${PROJECT_HEADERS}
    ${PROJECT_SOURCES}
)

plt_link_plt_libraries(${PROJECT_NAME} PRIVATE COMPONENTS Core)
plt_link_qt_libraries(${PROJECT_NAME} PRIVATE COMPONENTS Core)

target_include_directories(${PROJECT_NAME} PUBLIC include/)
target_compile_definitions(${PROJECT_NAME} PRIVATE TOOLKIT_LIBRARY)
