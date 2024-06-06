cmake_minimum_required(VERSION 3.6.0)

option(INSTALL_GTEST "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)" OFF)

add_subdirectory(plog)
add_subdirectory(googletest)
add_subdirectory(jsoncpp)