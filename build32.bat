@echo off
rmdir /Q /S build
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A Win32 -S .. -B "build32" -DVST3_SDK_ROOT=%VST3_SDK_ROOT_32BIT%
cmake --build build32 --config Release
cd ..
