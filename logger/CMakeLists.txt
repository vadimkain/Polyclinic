#----------------------------- CREATE LIBRARY -----------------------------#

cmake_minimum_required(VERSION 3.6.0)
set(PROJECT_NAME bukhtalogger)
project(${PROJECT_NAME})

set(
    LIB_SRC
    src/Logger.cpp
)

add_library(
    ${PROJECT_NAME}
    ${LIB_SRC}
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    include
)

#----------------------------- LINK LIBRARIES -----------------------------#

set(
    ALL_LIBS
    plog
)

target_link_libraries(
    ${PROJECT_NAME}
    ${ALL_LIBS}
)