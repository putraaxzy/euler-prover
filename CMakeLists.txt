cmake_minimum_required(VERSION 3.10)
project(EulerProof)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(euler ${SOURCES})

# Set output directory to build folder
set_target_properties(euler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    OUTPUT_NAME "euler"
)

if(WIN32)
    set_target_properties(euler PROPERTIES OUTPUT_NAME "euler.exe")
endif()

# Link libraries
target_link_libraries(euler OpenMP::OpenMP_CXX)

# Compiler-specific optimizations
if(MSVC)
    target_compile_options(euler PRIVATE /W4 /arch:AVX2)
else()
    target_compile_options(euler PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install target
install(TARGETS euler DESTINATION bin)
