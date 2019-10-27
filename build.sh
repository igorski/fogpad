#!/usr/bin/env bash
clear
echo "Flushing build caches and output folders"
rm -rf build
echo "Creating build folders"
mkdir build
cd build
echo "Building project"
echo "----------------"

if [ -n "${VST3_SDK_ROOT}" ]; then
  DVST3_SDK_ROOT="-DVST3_SDK_ROOT=${VST3_SDK_ROOT}"
fi

cmake "-DCMAKE_OSX_ARCHITECTURES=x86_64" ${DVST3_SDK_ROOT} ..
make
