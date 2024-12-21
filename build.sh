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

type="vst3"

# Parse arguments

while [[ "$#" -gt 0 ]]; do
    case $1 in
        --type) type="$2"; shift ;;
        --team_id) team_id="$2"; shift ;;
        --identity) identity="$2"; shift ;;
        --coresdk) coresdk="$2"; shift ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

# flag to build as VST2 supplied ? pass it to CMake configuration

if [ "$type" == "vst2" ]; then
    FLAGS="-DSMTG_CREATE_VST2_VERSION=ON"
elif [ "$type" == "au" ]; then
    if [ -z "$coresdk" ]; then
        echo "Path to CoreAudio SDK not supplied, usage: build.sh --type au --coresdk PATH_TO_CORE_AUDIO_SDK"
        exit
    fi
    FLAGS="-GXcode -DSMTG_CREATE_AU_VERSION=ON -DSMTG_COREAUDIO_SDK_PATH=$coresdk"
fi

if [ -z "$identity" ]; then
    cmake "-DCMAKE_OSX_ARCHITECTURES=x86_64;arm64" -DVST3_SDK_ROOT=${DVST3_SDK_ROOT} ${FLAGS} ..
else
    echo "Plugin will be signed using ${identity}"
    cmake "-DCMAKE_OSX_ARCHITECTURES=x86_64;arm64" -DVST3_SDK_ROOT=${DVST3_SDK_ROOT} ${FLAGS} -DSMTG_IOS_DEVELOPMENT_TEAM=${team_id} "-DSMTG_CODE_SIGN_IDENTITY_MAC=${identity}" ..
fi
cmake --build . --config Release

buildStatus=$?

if [ $buildStatus -eq 0 ]; then
    FILE="./VST3/fogpad.vst3"

    # code signing (macOS)
    
    if [ "$identity" ]; then
        if [ "$type" == "au" ]; then
            FILE="~/Library/Audio/Plug-Ins/Components/fogpad.component"
        fi
        codesign -s "${identity}" "${FILE}" --timestamp --deep --strict --options=runtime --force
        codesign --verify --deep --verbose "${FILE}"
    fi

    # VST2/AU specific operations

    if [ "$type" == "vst2" ]; then
        mv "${FILE}" ./VST3/fogpad.vst
    elif [ "$type" == "au" ]; then
        killall -9 AudioComponentRegistrar
        auval -v aufx rvb2 IGOR 
    fi
    echo "Plugin built successfully"
else
    echo "An error occurred during build of plugin"
fi

exit $buildStatus
