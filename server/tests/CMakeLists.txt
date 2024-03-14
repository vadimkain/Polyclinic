project(polyclinic_server_db_tests)

include_directories(
    ${GTEST_INCLUDE_DIRS}
    ${GMOCK_INCLUDE_DIRS}
)

set(
    SRC
    testRunner.cpp
)

add_executable(
    ${PROJECT_NAME}
    ${SRC}
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    gtest
    gmock
    gmock_main
)
