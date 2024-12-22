# FOGPAD

FogPad is a VST/AU plug-in which provides a reverb effect in which the reflections can be frozen, filtered, pitch shifted and ultimately disintegrated.

If you require some inspiration, it is used pretty much all over the [Moult](https://songwhip.com/drosophelia/moult) EP by Drosophelia. Alternatively, you can [read what others say](https://bedroomproducersblog.com/2019/11/18/igorski-fogpad/).

## On compatibility

### Build as VST 2.4

VST3 is great and all, but support across DAW's is poor (looking at a certain popular German product). You can however build as a VST2.4 plugin and enjoy it on a wider range of host platforms.

However: as of SDK 3.6.11, Steinberg no longer packages the required _./pluginterfaces/vst2.x_-folder inside the vst3sdk folder.
If you wish to build a VST2 plugin, copying the folder from an older SDK version _could_ work (verified 3.6.9. _vst2.x_ folders to work with SDK 3.7.6), though be aware
that you _need a license to target VST2_. You can view [Steinbergs rationale on this decision here](https://www.steinberg.net/en/newsandevents/news/newsdetail/article/vst-2-coming-to-an-end-4727.html).

Once your SDK is "setup" for VST2 you can pass "vst2" as an argument to the build.sh and build.bat files, e.g.:

```
sh build.sh vst2
```

### Compiling for both 32-bit and 64-bit architectures

Depending on your host software having 32-bit or 64-bit support (either Intel or M1), you can best compile for a wider range of architectures. To do so, updating the build shell scripts/batch files to contain the following:

**macOS:**

```
cmake -"DCMAKE_OSX_ARCHITECTURES=x86_64;arm64;i1386" ..
```

Which will allow you to compile a single, "fat" binary that supports all architectures (Intel, M1 and legacy 32-bit Intel). Note
the default behaviour is to compile for Intel and ARM CPU's.

**Windows:**

```
cmake.exe -G "Visual Studio 16 2019" -A Win64 -S .. -B "build64"
cmake.exe --build build64 --config Release

cmake.exe -G "Visual Studio 16 2019" -A Win32 -S .. -B "build32"
cmake.exe --build build32 --config Release
```

Which is a little more cumbersome as you compile separate binaries for the separate architectures.

Note that the above also needs to be done when building the Steinberg SDK (which for the Windows build implies that a separate build is created for each architecture).

While macOS has been fully 64-bit for the past versions, building for 32-bit provides the best backward
compatibility for older OS versions. And musicians are known to keep working systems at the cost of not
running an up to date system...

## Build instructions

The project uses [CMake](https://cmake.org) to generate the Makefiles and has been built and tested on macOS, Windows 10 and Linux (Ubuntu).

### Environment setup

Apart from requiring _CMake_ and a C(++) compiler such as _Clang_ or _MSVC_, the only other dependency is the [VST SDK from Steinberg](https://www.steinberg.net/en/company/developers.html) (the projects latest update requires SDK version 3.7.11).

Be aware that prior to building the plugin, the Steinberg SDK needs to be built from source as well. Following Steinbergs guidelines, the build target should be a _/build_-subfolder of the _/vst3sdk_-folder.

#### The simple way : installing a plugin-local version of the SDK

You can retrieve and build the SDK using the following commands.

macOS:

```
sh setup.sh mac
```

Linux:

```
sh setup.sh
```

Windows:

```
setup.bat
```

This will create a (Git ignored) subfolder in this repository folder with a prebuilt Steinberg SDK.

#### The flexible way : pointing towards an external SDK build / supporting VST2

If you wish to use a different SDK version (or wish to reuse an existing build elsewhere on your computer), you can invoke all
build scripts by providing the `VST3_SDK_ROOT` environment variable, like so:

```
VST3_SDK_ROOT=/path/to/prebuilt/VST3_SDK sh build.sh
```

To generate a release build of the SDK, execute the following commands from the root of the Steinberg SDK folder:

```
cd vst3sdk
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

The result being that _{VST3_SDK_ROOT}/build/lib/Release/_ will contain the Steinberg VST libraries required to build the plugin.

If you intend to build VST2 versions as well, run the following from the root of the Steinberg SDK folder (run the _.bat_ version instead of the _.sh_ version on Windows) prior to building the library:

```
./copy_vst2_to_vst3_sdk.sh
```

And if you are running Linux, you can easily resolve all dependencies by first running the following from the root of the Steinberg SDK folder:

```
./tools/setup_linux_packages_for_vst3sdk.sh
```

### Building the plugin

See the provided sheel scripts. The build output will be stored in _./build/VST3/fogpad.vst3_ as well as symbolically linked to your systems VST-plugin folder (on Unix).

#### Compiling on Unix systems:

```
sh build.sh
```

#### Compiling on Windows:

Assuming the Visual Studio Build Tools have been installed:

```
build.bat
```

### Running the plugin

You can copy the build output into your system VST(3) folder and run it directly in a VST host / DAW of your choice.

When debugging, you can also choose to run the plugin against Steinbergs validator and editor host utilities:

```
{VST3_SDK_ROOT}/build/bin/validator  build/VST3/fogpad.vst3
{VST3_SDK_ROOT}/build/bin/editorhost build/VST3/fogpad.vst3
```

### Build as Audio Unit (macOS only)

For this you will need a little extra preparation while building Steinberg SDK. Additionally, you will need the
"[CoreAudio SDK](https://developer.apple.com/library/archive/samplecode/CoreAudioUtilityClasses/Introduction/Intro.html)" and XCode. Execute the following instructions to build the SDK with Audio Unit support,
replace the value for `SMTG_COREAUDIO_SDK_PATH` with the actual installation location of the CoreAudio SDK:

```
sh setup.sh mac /path/to/CoreAudioUtilityClasses/CoreAudio
```

You will need to have your code signing set up appropriately. Assuming you have set up your Apple Developer account appropriately, you can find your sign identity like so:

```
security find-identity -p codesigning -v 
```

From which you can take your name and team id and pass them to the build script like so:

```
sh build.sh au TEAM_ID "YOUR_NAME"
```

The subsequent Audio Unit component will be located in _./build/bin/Release/Fogpad\ AUV3.app_.

You can validate the Audio Unit using Apple's _auval_ utility, by running _auval -v aufx rvb2 IGOR_ on the command line. Note that there is the curious behaviour that you might need to reboot before the plugin shows up, though you can force a flush of the Audio Unit cache at runtime by running _killall -9 AudioComponentRegistrar_.

#### Signing Audio Units

Codesign the .vst3 file inside the `Resources` folder (or take a presigned .vst3 build). Then codesign the .component separately.