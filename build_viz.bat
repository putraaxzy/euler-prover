@echo off
echo Building Euler Prover Visualization System...

if not exist build mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release

echo Build complete.
echo Run euler_viz.exe to start the visualization tool.
pause
