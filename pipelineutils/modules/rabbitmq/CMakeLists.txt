cmake_minimum_required(VERSION 3.19)
project(rabbitmq LANGUAGES CXX)

set(PROJECT_HEADERS
    src/buffer.h
    src/connectionhandler.h

    include/rabbitmq/queuesender.h
    include/rabbitmq/queuelistener.h
    include/rabbitmq/rabbitmqclient.h
    include/rabbitmq/rabbitmq_global.h
)

set(PROJECT_SOURCES
    src/buffer.cpp
    src/connectionhandler.cpp
    src/queuesender.cpp
    src/queuelistener.cpp
    src/rabbitmqclient.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${PROJECT_HEADERS}
    ${PROJECT_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE amqpcpp)
plt_link_qt_libraries(${PROJECT_NAME} PRIVATE COMPONENTS Core Network)

target_include_directories(${PROJECT_NAME} PUBLIC include/)
target_compile_definitions(${PROJECT_NAME} PRIVATE RABBIT_MQ_LIBRARY)
