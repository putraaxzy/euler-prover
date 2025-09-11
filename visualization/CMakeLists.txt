cmake_minimum_required(VERSION 3.10)

# Define visualization library
file(GLOB VIZ_SOURCES "3d_renderer/*.cpp" "*.cpp")

add_library(euler_visualization ${VIZ_SOURCES})
target_include_directories(euler_visualization PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(euler_visualization ${VTK_LIBRARIES})

# Make sure VTK modules are initialized properly
vtk_module_autoinit(
  TARGETS euler_visualization
  MODULES ${VTK_LIBRARIES}
)

# Set output directory
set_target_properties(euler_visualization PROPERTIES 
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
