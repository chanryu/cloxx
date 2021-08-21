cmake_minimum_required(VERSION 3.12)

project(cloxx)

file(GLOB CLOXX_SOURCES src/*.cpp generated_src/*.cpp)
add_executable(cloxx ${CLOXX_SOURCES})
target_include_directories(cloxx PRIVATE src generated_src)
set_target_properties(cloxx PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS OFF)
target_compile_options(cloxx PRIVATE -Werror -Wall -Wextra)