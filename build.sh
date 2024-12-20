#!/usr/bin/env bash
clear

# get current working directory

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
SOURCE="$(readlink "$SOURCE")"
[[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
BASEDIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

echo "Flushing build caches and output folders"
rm -rf build
echo "Creating build folders"
mkdir build
cd build
echo "Building project"
echo "----------------"

# env variable for VST3_SDK_ROOT set ? pass it to CMake configuration

if [ -n "${VST3_SDK_ROOT}" ]; then
    DVST3_SDK_ROOT=${VST3_SDK_ROOT}
else
    DVST3_SDK_ROOT="${BASEDIR}/vst3sdk"
fi

echo "Building using VST3 SDK located at ${DVST3_SDK_ROOT}"

# flag to build as VST2 supplied ? pass it to CMake configuration

if [ "$1" == "vst2" ]; then
    FLAGS="-DSMTG_CREATE_VST2_VERSION=ON"
fi

if [ "$1" == "au" ]; then
    FLAGS="-GXcode -DSMTG_CREATE_AU_VERSION=ON"
fi

cmake "-DCMAKE_OSX_ARCHITECTURES=x86_64;arm64" -DVST3_SDK_ROOT=${DVST3_SDK_ROOT} ${FLAGS} ..
cmake --build . --config Release

buildStatus=$?

if [ $buildStatus -eq 0 ]; then
    if [ "$1" == "vst2" ]; then
        mv ./VST3/fogpad.vst3 ./VST3/fogpad.vst
    fi
    echo "Plugin built successfully"
else
    echo "An error occurred during build of plugin"
fi

exit $buildStatus
