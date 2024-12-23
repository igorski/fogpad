@echo off
cls

echo "Retrieving Steinberg VST3 SDK..."
git clone --recursive https://github.com/steinbergmedia/vst3sdk.git --branch v3.7.11_build_10

cd vst3sdk
rmdir /Q /S build
mkdir build
cd build

cmake.exe -G"Visual Studio 16 2019" ..
cmake --build . --config Release

cd ..
