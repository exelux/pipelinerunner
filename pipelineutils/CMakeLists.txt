cmake_minimum_required(VERSION 3.19)
project(pipelineutils LANGUAGES CXX)

option(PIPELINEUTILS_MODULE_RABBITMQ "RabbitMQ module" ON)
option(PIPELINEUTILS_MODULE_TOOLKIT "Toolkit module" ON)

if(PIPELINEUTILS_MODULE_RABBITMQ)
    add_subdirectory(modules/rabbitmq)
    add_subdirectory(3rd_party/amqpcpp)
endif()    

if (PIPELINEUTILS_MODULE_TOOLKIT)
    add_subdirectory(modules/toolkit)
endif()
