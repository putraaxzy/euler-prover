cmake_minimum_required(VERSION 3.10)
project(EulerProof)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find required packages
find_package(OpenMP REQUIRED)
find_package(VTK REQUIRED)
add_compile_definitions(VTK_FOUND)

include_directories(include ${VTK_INCLUDE_DIRS})

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
target_link_libraries(euler OpenMP::OpenMP_CXX ${VTK_LIBRARIES})

# Compiler-specific optimizations
if(MSVC)
    target_compile_options(euler PRIVATE /W4 /arch:AVX2)
else()
    target_compile_options(euler PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install target
install(TARGETS euler DESTINATION bin)

# Visualization examples
add_subdirectory(visualization)

# Create demo executables
file(GLOB TOPOLOGY_DEMO_SOURCES "visualization/examples/topology_demos/*.cpp")
add_executable(topology_demo ${TOPOLOGY_DEMO_SOURCES})
target_link_libraries(topology_demo euler ${VTK_LIBRARIES})
set_target_properties(topology_demo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(GLOB COMPLEX_DEMO_SOURCES "visualization/examples/complex_analysis_demos/*.cpp")
add_executable(complex_analysis_demo ${COMPLEX_DEMO_SOURCES})
target_link_libraries(complex_analysis_demo euler ${VTK_LIBRARIES})
set_target_properties(complex_analysis_demo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(GLOB NUMBER_DEMO_SOURCES "visualization/examples/number_theory_demos/*.cpp")
add_executable(number_theory_demo ${NUMBER_DEMO_SOURCES})
target_link_libraries(number_theory_demo euler ${VTK_LIBRARIES})
set_target_properties(number_theory_demo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Main visualization launcher
add_executable(euler_viz visualization/euler_viz.cpp)
set_target_properties(euler_viz PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# VTK specific settings
vtk_module_autoinit(
  TARGETS euler topology_demo complex_analysis_demo number_theory_demo euler_viz
  MODULES ${VTK_LIBRARIES}
)

# Link VTK to main euler executable
target_link_libraries(euler ${VTK_LIBRARIES})
target_link_libraries(euler_viz ${VTK_LIBRARIES})
