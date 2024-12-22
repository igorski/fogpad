#!/usr/bin/env bash
clear

echo "Retrieving Steinberg VST3 SDK..."
git clone --recursive https://github.com/steinbergmedia/vst3sdk.git --branch v3.7.11_build_10

echo "--------------------------------"
echo "Setting up Steinberg VST3 SDK..."

cd vst3sdk
rm -rf build
mkdir build
cd build

echo "---------------------"

# Parse arguments

while [[ "$#" -gt 0 ]]; do
    case $1 in
        --platform) platform="$2"; shift ;;
        --coresdk) coresdk="$2"; shift ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

if [ "$platform" == "mac" ]; then
    echo "Building for macOS..."
    if [ "$coresdk" ]; then
        echo "Building with CoreAudio support. Library specified to be at $coresdk"
        FLAGS="-DSMTG_COREAUDIO_SDK_PATH=$coresdk"
    fi
    cmake -GXcode -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" ${FLAGS} ..
else
    echo "Building for Linux..."
    make
fi

cmake --build . --config Release