cmake_minimum_required(VERSION 3.6.0)
set(PROJECT_NAME db_connector_server)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

set (CPP_COMPILATION_FLAGS
    "-lpqxx"
    "-lpq"
)
# rewrite list to string;
string (REPLACE ";" " " CPP_COMPILATION_FLAGS "${CPP_COMPILATION_FLAGS}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPP_COMPILATION_FLAGS}")

add_executable(
    ${PROJECT_NAME}
    main.cpp
)

#----------------------------- LINK LIBRARIES -----------------------------#

add_subdirectory(common)
add_subdirectory(db)
add_subdirectory(tests)

set(
    ALL_LIBS
    polyclinic_server_db
    bukhtalogger
)

target_link_libraries(
    ${PROJECT_NAME}
    ${ALL_LIBS}
)