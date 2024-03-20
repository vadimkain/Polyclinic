#--------------------------- MAIN CONFIGURATION ---------------------------#

cmake_minimum_required(VERSION 3.6.0)
set(PROJECT_NAME polyclinic)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)
    
# Set this option to generate compile_commands.json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(logger)