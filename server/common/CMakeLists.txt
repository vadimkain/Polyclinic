#----------------------------- CREATE LIBRARY -----------------------------#

cmake_minimum_required(VERSION 3.6.0)
set(PROJECT_NAME polyclinic_server_common)
project(${PROJECT_NAME})

file(GLOB LIB_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(
    ${PROJECT_NAME}
    STATIC
    ${LIB_SRC}
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

#----------------------------- LINK LIBRARIES -----------------------------#

set(
    ALL_LIBS
    bukhtalogger
)

target_link_libraries(
    ${PROJECT_NAME}
    ${ALL_LIBS}
)