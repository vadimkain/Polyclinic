#----------------------------- CREATE LIBRARY -----------------------------#

cmake_minimum_required(VERSION 3.6.0)
set(PROJECT_NAME polyclinic_server_common)
project(${PROJECT_NAME})


set(
    HEADERS_INCLUDES
    config.hpp
)

add_library(
    ${PROJECT_NAME}
    INTERFACE
    ${HEADERS_INCLUDES}
)

target_include_directories(
    ${PROJECT_NAME}
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)