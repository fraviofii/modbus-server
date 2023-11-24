cmake_minimum_required(VERSION 3.1)

set(VERSION 1.0.0)

project(modbus-server VERSION ${VERSION})

include(cmake/common.cmake)

add_subdirectory(lib/sqlite)
add_subdirectory(lib)

message(STATUS "LIBMODBUS_BINARY_DIR: ${LIBMODBUS_BINARY_DIR}")

add_subdirectory(src)
