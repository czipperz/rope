cmake_minimum_required(VERSION 2.6)
project(ROPE)

# allow loading of packages based on scripts in `cmake` directory
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      ${ROPE_SOURCE_DIR}/cmake)

set(files ${ROPE_SOURCE_DIR}/src/rope.c)

add_definitions("-Wincompatible-pointer-types" "-Wall"
  "-Wextra" "-Wpedantic" "-Wno-error=unused-parameter"
  "-Wno-error=format-extra-args" "-Wno-unused-function"
  "-Wno-unused-variable")

add_library(rope STATIC ${files})
install(TARGETS rope DESTINATION lib)

add_executable(test_rope ${files} ${ROPE_SOURCE_DIR}/src/test_main.c)
target_compile_definitions(test_rope PRIVATE "TEST_MODE")

include_directories(${ROPE_SOURCE_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
