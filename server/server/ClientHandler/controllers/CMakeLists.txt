#----------------------------- CREATE LIBRARY -----------------------------#

cmake_minimum_required(VERSION 3.6.0)
set(PROJECT_NAME polyclinic_server_code_client_handler_controllers)
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
    bukhtalogger
    polyclinic_server_db
    polyclinic_server_code_client_handler_models
    polyclinic_server_code_server_starter_models
    polyclinic_server_code_client_handler_view
    polyclinic_server_code_context_handler_view
    # external libs:
    jsoncpp_lib
)

target_link_libraries(
    ${PROJECT_NAME}
    ${ALL_LIBS}
)