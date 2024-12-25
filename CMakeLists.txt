###################################
# CMake Project for FOGPAD Plugin #
###################################

cmake_minimum_required(VERSION 3.19)
set(CMAKE_COLOR_MAKEFILE ON)

# uncomment to build as VST2.4 instead of VST3.0 (provides wider DAW compatibility), not supported on Linux
#set(SMTG_CREATE_VST2_VERSION "Use VST2" ON)

project(Fogpad)
set(PROJECT_VERSION 1)
set(target fogpad)
set(copyright "igorski.nl 2019-2024")
set(major_version 1)
set(minor_version 0)
set(release_number 3)
set(build_number 1) # TODO supply through CLI (build number is not included in public facing version strings)
set(version_string "${major_version}.${minor_version}.${release_number}") # also see audiounitconfig.h#kAUcomponentVersion and related Info.plist
set(SMTG_CREATE_MODULE_INFO false)

#####################
# Compiler settings #
#####################

set(CMAKE_CXX_STANDARD 17)
add_definitions(-DNDEBUG)
add_compile_definitions(PLUGIN_COPYRIGHT=${copyright})
add_compile_definitions(PLUGIN_MAJOR_VERSION=${major_version})
add_compile_definitions(PLUGIN_MINOR_VERSION=${minor_version})
add_compile_definitions(PLUGIN_RELEASE_NUMBER=${release_number})
add_compile_definitions(PLUGIN_BUILD_NUMBER=${build_number})

if(MSVC)
    add_definitions(/D _CRT_SECURE_NO_WARNINGS)
endif()

if(UNIX)
    if(APPLE)
        if (XCODE)
            set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17")
            set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
        else()
            set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -stdlib=libc++")
            link_libraries(c++)
        endif()
        # support Yosemite and up
        set(CMAKE_OSX_SYSROOT macosx10.13)
        set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")
    else()
        set(LINUX true)
        add_definitions( -D__cdecl= )
        set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wno-multichar")
        link_libraries(stdc++fs pthread dl)
    endif()
else()
    ## spotted to not be set by default on VS CLI. Here we assume any non-Unix
    ## OS MUST be Windows. Then again that's the only other intended target for this project...
    set(WIN true)
endif()

############
# Includes #
############

list(APPEND CMAKE_MODULE_PATH "${VST3_SDK_ROOT}/cmake/modules")

# include(SMTG_AAXSupport)
include(SMTG_AddVST3Library)
include(SMTG_AddVST3Options)
include(SMTG_Bundle)
include(SMTG_CoreAudioSupport)
include(SMTG_ExportedSymbols)
include(SMTG_Global)
include(SMTG_PlatformIOS)
include(SMTG_PlatformToolset)
include(SMTG_PrefixHeader)
include(SMTG_UniversalBinary)
include(SMTG_VstGuiSupport)

#########################
# Steinberg VST sources #
#########################

set(VSTSDK_PLUGIN_SOURCE
    ${VST3_SDK_ROOT}/public.sdk/source/common/commoniids.cpp
    ${VST3_SDK_ROOT}/public.sdk/source/vst/vstaudioeffect.cpp
    ${VST3_SDK_ROOT}/public.sdk/source/vst/vstaudioprocessoralgo.h
    ${VST3_SDK_ROOT}/public.sdk/source/vst/vsteditcontroller.h
    ${VST3_SDK_ROOT}/pluginterfaces/base/ibstream.h
    ${VST3_SDK_ROOT}/pluginterfaces/base/ustring.h
    ${VST3_SDK_ROOT}/pluginterfaces/vst/ivstevents.h
    ${VST3_SDK_ROOT}/pluginterfaces/vst/ivstparameterchanges.h
    ${VST3_SDK_ROOT}/pluginterfaces/vst/vstpresetkeys.h
)

set(vst2_sources
    ${VST3_SDK_ROOT}/public.sdk/source/vst/vst2wrapper/vst2wrapper.sdk.cpp
    src/vstentry_vst2.cpp
)
if(APPLE)
     set(vst2_sources
         ${vst2_sources}
         ${VST3_SDK_ROOT}/public.sdk/source/common/threadchecker_mac.mm
     )
 elseif(WIN)
     set(vst2_sources
         ${vst2_sources}
         ${VST3_SDK_ROOT}/public.sdk/source/common/threadchecker_win32.cpp
     )
 endif()

##########################
# Plugin project sources #
##########################

set(vst_sources
    src/global.h
    src/allpass.h
    src/allpass.cpp
    src/audiobuffer.h
    src/audiobuffer.cpp
    src/bitcrusher.h
    src/bitcrusher.cpp
    src/comb.h
    src/comb.cpp
    src/decimator.h
    src/decimator.cpp
    src/filter.h
    src/filter.cpp
    src/lfo.h
    src/lfo.cpp
    src/limiter.h
    src/limiter.cpp
    src/paramids.h
    src/reverbprocess.h
    src/reverbprocess.cpp
    src/vst.h
    src/vst.cpp
    src/vstentry.cpp
    src/version.h
    src/ui/controller.h
    src/ui/controller.cpp
    src/ui/uimessagecontroller.h
    ${VSTSDK_PLUGIN_SOURCE}
)

# add the VST2 source files when compiling a VST2 (not supported on Linux)
if(SMTG_CREATE_VST2_VERSION)
    if(APPLE OR WIN)
        set(vst_sources ${vst_sources} ${vst2_sources})
    endif()
endif()

set(vst_resources
    "resource/background.png"
    "resource/version.png"
)
set(vst_ui_descr "resource/plugin.uidesc")

#######
# VST #
#######

smtg_add_vst3plugin(${target} ${vst_sources})
smtg_target_configure_version_file(${target})

## include Steinberg libraries

set(steinberg_libs "base" "pluginterfaces" "sdk" "vstgui" "vstgui_support" "vstgui_uidescription")
include_directories(${VST3_SDK_ROOT})
foreach(lib IN ITEMS ${steinberg_libs})
    if(UNIX)
        target_link_libraries(${target} PRIVATE ${VST3_SDK_ROOT}/build/lib/Release/lib${lib}.a)
    elseif(WIN)
        target_link_libraries(${target} PRIVATE ${VST3_SDK_ROOT}/build/lib/Release/${lib}.lib)
    endif()
endforeach(lib)

## Include Steinberg VSTGUI

target_include_directories(${target} PUBLIC ${VST3_SDK_ROOT}/vstgui4)
target_sources(${target} PRIVATE
    ${VST3_SDK_ROOT}/vstgui4/vstgui/vstgui_uidescription.cpp
    ${VST3_SDK_ROOT}/vstgui4/vstgui/plugin-bindings/vst3editor.cpp
    ${VST3_SDK_ROOT}/public.sdk/source/vst/vstguieditor.cpp
)

## include macOS specific libraries

IF (APPLE)
    target_sources (${target} PRIVATE
        ${VST3_SDK_ROOT}/public.sdk/source/main/macmain.cpp
    )
    if(XCODE)
        target_link_libraries(${target} PRIVATE "-framework Cocoa" "-framework OpenGL" "-framework Accelerate" "-framework QuartzCore" "-framework Carbon")
    else()
        find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
        find_library(COCOA_FRAMEWORK Cocoa)
        find_library(OPENGL_FRAMEWORK OpenGL)
        find_library(ACCELERATE_FRAMEWORK Accelerate)
        find_library(QUARTZCORE_FRAMEWORK QuartzCore)
        find_library(CARBON_FRAMEWORK Carbon)
        find_library(EXPAT Expat)
        target_link_libraries(${target} PRIVATE ${COREFOUNDATION_FRAMEWORK} ${COCOA_FRAMEWORK} ${OPENGL_FRAMEWORK} ${ACCELERATE_FRAMEWORK} ${QUARTZCORE_FRAMEWORK} ${CARBON_FRAMEWORK} ${EXPAT})
    endif()
    set_target_properties(${target} PROPERTIES
        BUNDLE true
        BUNDLE_EXTENSION "vst3"
        XCODE_ATTRIBUTE_WRAPPER_EXTENSION "vst3"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/mac/Info.plist"
        MACOSX_BUNDLE_BUNDLE_NAME "${target}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "nl.igorski.vst.${target}"
        MACOSX_BUNDLE_ICON_FILE ""
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${version_string}"
        MACOSX_BUNDLE_COPYRIGHT "${copyright}"
    )
endif()

## include Linux specific libraries

if (LINUX)
    target_sources (${target} PRIVATE
        ${VST3_SDK_ROOT}/public.sdk/source/main/linuxmain.cpp
    )
    set(VSTGUI_LTO_COMPILER_FLAGS "-O3 -flto")
    set(VSTGUI_LTO_LINKER_FLAGS "")
    find_package(X11 REQUIRED)
    find_package(Freetype REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBXCB REQUIRED xcb)
    pkg_check_modules(LIBXCB_UTIL REQUIRED xcb-util)
    pkg_check_modules(LIBXCB_CURSOR REQUIRED xcb-cursor)
    pkg_check_modules(LIBXCB_KEYSYMS REQUIRED xcb-keysyms)
    pkg_check_modules(LIBXCB_XKB REQUIRED xcb-xkb)
    pkg_check_modules(LIBXKB_COMMON REQUIRED xkbcommon)
    pkg_check_modules(LIBXKB_COMMON_X11 REQUIRED xkbcommon-x11)
    set(LINUX_LIBRARIES
        ${X11_LIBRARIES}
        ${FREETYPE_LIBRARIES}
        ${LIBXCB_LIBRARIES}
        ${LIBXCB_UTIL_LIBRARIES}
        ${LIBXCB_CURSOR_LIBRARIES}
        ${LIBXCB_KEYSYMS_LIBRARIES}
        ${LIBXCB_XKB_LIBRARIES}
        ${LIBXKB_COMMON_LIBRARIES}
        ${LIBXKB_COMMON_X11_LIBRARIES}
        cairo
        fontconfig
        dl
    )
    target_link_libraries(${target} PRIVATE ${LINUX_LIBRARIES})
endif()

## Include Windows specific libraries

if(WIN)
    target_sources(${target} PRIVATE
        ${VST3_SDK_ROOT}/public.sdk/source/main/dllmain.cpp
#        ${VST3_SDK_ROOT}/vstgui4/vstgui/vstgui_win32.cpp
    )
endif()

## Add the resource files to the bundle

smtg_target_add_plugin_resources(${target}
    RESOURCES ${vst_ui_descr} ${vst_resources}
)

if(APPLE)
    ##############
    # Audio Unit #
    ##############
    if (XCODE AND SMTG_CREATE_AU_VERSION)
        message(STATUS "SMTG_CREATE_AU_VERSION is set. An Audio Unit version of the plug-in will be created.")
        smtg_target_codesign(${target} ${SMTG_IOS_DEVELOPMENT_TEAM} ${SMTG_CODE_SIGN_IDENTITY_MAC})
        add_subdirectory(mac/audio-unit)
        create_audio_unit(${target})
    else()
        smtg_target_set_bundle(${target} INFOPLIST "${CMAKE_CURRENT_SOURCE_DIR}/mac/Info.plist" PREPROCESS)
        # adding PkgInfo at root level makes plugin appear as a file instead of folder
        smtg_target_add_plugin_resources(${target} RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/mac/PkgInfo" OUTPUT_SUBDIRECTORY "../")
    endif()
    smtg_target_set_bundle(${target}
        BUNDLE_IDENTIFIER "nl.igorski.${target}"
        COMPANY_NAME "igorski.nl"
    )
elseif(WIN)
    target_sources(${target} PRIVATE resource/plugin.rc)
endif()

if (SMTG_CREATE_VST2_VERSION)
    message(STATUS "SMTG_CREATE_VST2_VERSION is set. A VST 2 version of the plug-in will be created.")
    if(XCODE)
        # fix missing VSTPluginMain symbol when also building VST 2 version
        set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_EXPORTED_SYMBOLS_FILE "")
    endif()
    if (WIN)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    endif()
endif()

######################
# Installation paths #
######################

if(APPLE)
    install(TARGETS ${target}
        DESTINATION "$ENV{HOME}/Library/Audio/Plug-Ins/VST"
    )
elseif(WIN32)
    install(TARGETS ${target}
        DESTINATION "C:/Program Files (x86)/Common Files/VST3/"
    )
elseif(WIN)
    install(TARGETS ${target}
        DESTINATION "C:/Program Files/Common Files/VST3/"
    )
elseif(LINUX)
    install(TARGETS ${target}
        DESTINATION "/usr/lib/lxvst"
    )
endif()