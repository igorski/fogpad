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

cmake -GXcode ${DVST3_SDK_ROOT} -DJAMBA_ENABLE_AUDIO_UNIT=ON ${BASEDIR}

# build Audio Unit using Jamba in Release mode

sh jamba.sh build-au -r
sh jamba.sh install-au

cd ..
