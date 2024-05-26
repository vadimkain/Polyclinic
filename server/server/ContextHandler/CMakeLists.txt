#----------------------------- CREATE LIBRARY -----------------------------#

cmake_minimum_required(VERSION 3.6.0)
set(PROJECT_NAME polyclinic_server_code_context_handler)
project(${PROJECT_NAME})

add_library(
    ${PROJECT_NAME}
    INTERFACE
)

#----------------------------- LINK LIBRARIES -----------------------------#

add_subdirectory(controllers)
add_subdirectory(models)
add_subdirectory(view)

set(
    ALL_LIBS
    polyclinic_server_code_context_handler_models
    polyclinic_server_code_context_handler_controllers
    polyclinic_server_code_context_handler_view
)

target_link_libraries(
    ${PROJECT_NAME}
    INTERFACE
    ${ALL_LIBS}
)