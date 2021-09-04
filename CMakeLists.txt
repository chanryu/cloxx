cmake_minimum_required(VERSION 3.12)

project(cloxx)

#file(GLOB CLOXX_SOURCES src/*.cpp gen/*.cpp)
file(GLOB CLOXX_SOURCES src/main.cpp src/Token.cpp src/LoxObject.cpp src/Parser.cpp src/Resolver.cpp gen/*.cpp)
add_executable(cloxx ${CLOXX_SOURCES})
target_include_directories(cloxx PRIVATE src gen)
set_target_properties(cloxx PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS OFF)
target_compile_options(cloxx PRIVATE -Werror -Wall -Wextra)
target_compile_definitions(cloxx PRIVATE $<$<CONFIG:Debug>:CLOXX_GC_DEBUG=1>)