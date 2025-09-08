@echo off
echo ==========================================
echo Building Euler Computational Proof System
echo ==========================================

if not exist build mkdir build

echo Cleaning previous builds...
if exist build\*.exe del build\*.exe
if exist build\*.o del build\*.o

echo.
echo Compiling standard version...
g++ -std=c++17 -O3 -DNDEBUG -Wall -Wextra -Iinclude src/*.cpp -o build/euler.exe

if exist build\euler.exe (
    echo ✅ Standard build successful: build\euler.exe
) else (
    echo ❌ Standard build failed!
    exit /b 1
)

echo.
echo Compiling extreme performance version...
g++ -std=c++17 -O3 -DNDEBUG -march=native -flto -funroll-loops -Wall -Wextra -Iinclude src/*.cpp -o build/euler_extreme.exe

if exist build\euler_extreme.exe (
    echo ✅ Extreme build successful: build\euler_extreme.exe
) else (
    echo ❌ Extreme build failed!
)

echo.
echo ==========================================
echo BUILD COMPLETE
echo ==========================================
echo Standard usage:
echo   build\euler.exe proof number 100 10 4
echo   build\euler.exe proof complex 5000 8
echo   build\euler.exe proof topology 3
echo   build\euler.exe proof ultra 1.0 50 all
echo.
echo Extreme performance:
echo   build\euler_extreme.exe proof number 50000 200 8
echo   build\euler_extreme.exe proof complex 50000000 22 8
echo   build\euler_extreme.exe proof ultra 2.5 60 all
echo ==========================================