#!/usr/bin/env bash
clear

echo "Retrieving Steinberg VST3 SDK..."
#git clone --recursive https://github.com/steinbergmedia/vst3sdk.git --branch v3.7.11_build_10

echo "--------------------------------"
echo "Setting up Steinberg VST3 SDK..."

cd vst3sdk
rm -rf build
mkdir build
cd build

echo "---------------------"

if [ "$1" == "mac" ]; then
    echo "Building for macOS..."
    if [ "$2" != "" ]; then
        echo "Building with CoreAudio support. Library specified to be at $2"
        FLAGS="-DSMTG_COREAUDIO_SDK_PATH=$2"
    fi
    cmake -GXcode -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" ${FLAGS} ..
else
    echo "Building for Linux..."
    make
fi

cmake --build . --config Release
