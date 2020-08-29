@echo off
rmdir /Q /S build
mkdir build
cd build
cmake.exe -G"Visual Studio 16 2019" -DVST3_SDK_ROOT=%VST3_SDK_ROOT% ..
cmake --build . --config Release
cd ..