cmake_minimum_required(VERSION 3.10)

project(RITIMAGINE_Core)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(collections)
add_subdirectory(network)
add_subdirectory(game)
add_subdirectory(i2c)
add_subdirectory(serial)
add_subdirectory(test)
add_subdirectory(utils)
