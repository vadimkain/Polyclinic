#----------------------------- CREATE LIBRARY -----------------------------#

cmake_minimum_required(VERSION 3.6.0)
set(PROJECT_NAME polyclinic_server_code)
project(${PROJECT_NAME})

add_library(
    ${PROJECT_NAME}
    INTERFACE
)

#----------------------------- LINK LIBRARIES -----------------------------#

add_subdirectory(ClientHandler)
add_subdirectory(ServerStarter)
add_subdirectory(ContextHandler)

set(
    ALL_LIBS
    polyclinic_server_code_client_handler
    polyclinic_server_code_server_starter
    polyclinic_server_code_context_handler
)

target_link_libraries(
    ${PROJECT_NAME}
    INTERFACE
    ${ALL_LIBS}
)