cmake_minimum_required(VERSION 3.10)

project(RITIMAGINE_Core)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_subdirectory(collections)
add_subdirectory(network)
add_subdirectory(game)
add_subdirectory(i2c)
add_subdirectory(serial)
add_subdirectory(test)
add_subdirectory(utils)

add_library(core STATIC $<TARGET_OBJECTS:Collections>
        $<TARGET_OBJECTS:Network>
        $<TARGET_OBJECTS:I2C>
        #$<TARGET_OBJECTS:Serial>
        #$<TARGET_OBJECTS:Utils>
        )

#Get the git commit hash for the project
execute_process(COMMAND git rev-parse --short HEAD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE _GIT_HASH)
message(STATUS "git Commit Hash: ${_GIT_HASH}")
add_compile_definitions(core GIT_COMMIT_HASH "0x${_GIT_HASH}")

