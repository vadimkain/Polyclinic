#----------------------------- CREATE LIBRARY -----------------------------#

cmake_minimum_required(VERSION 3.6.0)
set(PROJECT_NAME polyclinic_server_db)
project(${PROJECT_NAME})

file(GLOB LIB_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(
    ${PROJECT_NAME}
    ${LIB_SRC}
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    include
)

#----------------------------- LINK LIBRARIES -----------------------------#


set(
    ALL_LIBS
    pqxx
    pq
    polyclinic_server_common
    bukhtalogger
)

target_link_libraries(
    ${PROJECT_NAME}
    ${ALL_LIBS}
)
