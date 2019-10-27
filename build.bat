@echo off
rmdir /Q /S build
mkdir build
cd build
cmake.exe -G"Visual Studio 15 2017 Win64" ..
cmake --build . --config Release
cd ..