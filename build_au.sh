#!/usr/bin/env bash
clear
echo "Flushing build caches and output folders"
rm -rf build
echo "Building project"
echo "----------------"

# from http://stackoverflow.com/questions/59895/can-a-bash-script-tell-what-directory-its-stored-in
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
BASEDIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

mkdir -p build
cd build

if [ -n "${VST3_SDK_ROOT}" ]; then
  DVST3_SDK_ROOT="-DVST3_SDK_ROOT=${VST3_SDK_ROOT}"
fi

# run CMake using Audio Unit build target
cmake -GXcode ${DVST3_SDK_ROOT} "-DCMAKE_OSX_ARCHITECTURES=\"arm64,x86_64\"" "-DCREATE_AUDIO_UNIT=ON" "${BASEDIR}/audio-unit"
cmake --build . --config Release

cd ..
