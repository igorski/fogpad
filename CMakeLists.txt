###################################
# CMake Project for FOGPAD Plugin #
###################################

# uncomment to build as VST2.4 instead of VST3.0 (provides wider DAW compatibility)
#set(SMTG_CREATE_VST2_VERSION "Use VST2" ON)

# uncomment to build Audio Unit (requires macOS and Xcode)
#option(JAMBA_ENABLE_AUDIO_UNIT "Enable Audio Unit" ON)

#######################
# DO NOT CHANGE BELOW #
#######################

project(FOGPAD)
cmake_minimum_required(VERSION 3.12)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)

# compiler flags
add_definitions(-DNDEBUG)

if(MSVC)
    add_definitions(/D _CRT_SECURE_NO_WARNINGS)
endif()

###############
# Unix builds #
###############

if(UNIX)
    if(XCODE)
       set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14")
       set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    elseif(APPLE)
       set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
       set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++")
       link_libraries(c++)
       # support Yosemite and up
       set(CMAKE_OSX_SYSROOT macosx10.10)
       set(CMAKE_OSX_DEPLOYMENT_TARGET "10.10")
    else()
        add_definitions( -D__cdecl= )
        set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wno-multichar")
        link_libraries(stdc++fs pthread dl)
    endif()
endif()


#-------------------------------------------------------------------------------
# Includes
#-------------------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${VST3_SDK_ROOT}/cmake/modules")

include(Global)
include(AddVST3Library)
include(Bundle)
include(ExportedSymbols)
include(PrefixHeader)
include(PlatformIOS)
include(PlatformToolset)
include(CoreAudioSupport)
include(AAXSupport)
include(VstGuiSupport)
include(UniversalBinary)
include(AddVST3Options)

###############
# VST sources #
###############

set(VSTSDK_INCLUDE_DIR ${VST3_SDK_ROOT})
set(VSTSDK_PLUGIN_SOURCE
  ${VST3_SDK_ROOT}/public.sdk/source/vst/vstaudioeffect.cpp
  ${VST3_SDK_ROOT}/public.sdk/source/vst/vstaudioprocessoralgo.h
  ${VST3_SDK_ROOT}/public.sdk/source/vst/vsteditcontroller.h
  ${VST3_SDK_ROOT}/pluginterfaces/base/ibstream.h
  ${VST3_SDK_ROOT}/pluginterfaces/base/ustring.h
  ${VST3_SDK_ROOT}/pluginterfaces/vst/ivstevents.h
  ${VST3_SDK_ROOT}/pluginterfaces/vst/ivstparameterchanges.h
  ${VST3_SDK_ROOT}/pluginterfaces/vst/vstpresetkeys.h
)

#if (SMTG_CREATE_VST2_VERSION)
    set(fogpad_vst2_sources
        ${VST3_SDK_ROOT}/public.sdk/source/common/memorystream.cpp
        ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/eventlist.cpp
        ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/hostclasses.cpp
        ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/parameterchanges.cpp
        ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/processdata.cpp
        ${VST3_SDK_ROOT}/public.sdk/source/vst/vst2wrapper/vst2wrapper.cpp
        ${VST3_SDK_ROOT}/public.sdk/source/vst/vst2wrapper/vst2wrapper.h
        ${VST3_SDK_ROOT}/public.sdk/source/vst2.x/audioeffect.cpp
        ${VST3_SDK_ROOT}/public.sdk/source/vst2.x/audioeffectx.cpp
        src/vstentry_vst2.cpp
    )
#endif()

###################
# Project sources #
###################

set(fogpad_sources
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

# VST2 is only defined for macOS and Windows
if(MAC OR WIN)
    set(fogpad_sources ${fogpad_sources} ${fogpad_vst2_sources})
endif()

set(target fogpad)

##############
# Audio Unit #
##############

if (JAMBA_ENABLE_AUDIO_UNIT)
    # To disable download/update, you can set those 2 variables to local folders
    unset(JAMBA_ROOT_DIR)
    #set(JAMBA_ROOT_DIR pongasoft/jamba)
    unset(GOOGLETEST_ROOT_DIR)
    #set(GOOGLETEST_ROOT_DIR google/googletest)

    include(jamba.cmake)

    set(CPP_SOURCES src)

    configure_file(${CPP_SOURCES}/version.h ${CMAKE_BINARY_DIR}/generated/version.h)
    include_directories(${CMAKE_BINARY_DIR}/generated/)

    jamba_add_vst3plugin(${target} "${fogpad_sources}")
    target_include_directories(${target} PUBLIC ${VST3_SDK_ROOT}/vstgui4)
    target_link_libraries(${target} PRIVATE base sdk vstgui_support jamba)

    ###################################################
    # Resources - add your resources here...
    ###################################################

    jamba_add_vst3_resource(${target} DATA "fogpad.uidesc")
    jamba_add_vst3_resource(${target} PNG "background.png")

    # this must be called after you are done calling jamba_add_vst3_resource for all resources
    jamba_gen_vst3_resources(${target} "FogPad")

    jamba_dev_scripts(${target})

else()

#######
# VST #
#######

smtg_add_vst3plugin(${target} ${VST3_SDK_ROOT} ${fogpad_sources})

include_directories(${VSTSDK_INCLUDE_DIR})
target_link_libraries(${target} ${VSTSDK_LIBRARIES})
target_include_directories(${target} PUBLIC ${VST3_SDK_ROOT}/vstgui4)
if (UNIX)
    target_link_libraries(${target} PRIVATE ${VST3_SDK_ROOT}/build/lib/libbase.a)
    target_link_libraries(${target} PRIVATE ${VST3_SDK_ROOT}/build/lib/libsdk.a)
    target_link_libraries(${target} PRIVATE ${VST3_SDK_ROOT}/build/lib/libvstgui_support.a)
    target_link_libraries(${target} PRIVATE ${VST3_SDK_ROOT}/build/lib/libvstgui_uidescription.a)
    target_link_libraries(${target} PRIVATE ${VST3_SDK_ROOT}/build/lib/libvstgui.a)
endif()
if (WIN)
    target_link_libraries(${target} PRIVATE ${VST3_SDK_ROOT}/build/lib/Release/base.lib)
    target_link_libraries(${target} PRIVATE ${VST3_SDK_ROOT}/build/lib/Release/sdk.lib)
    target_link_libraries(${target} PRIVATE ${VST3_SDK_ROOT}/build/lib/Release/vstgui_support.lib)
    target_link_libraries(${target} PRIVATE ${VST3_SDK_ROOT}/build/lib/Release/vstgui_uidescription.lib)
    target_link_libraries(${target} PRIVATE ${VST3_SDK_ROOT}/build/lib/Release/vstgui.lib)
endif()

# macOS specific libraries

IF (APPLE)
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
    set_target_properties(fogpad PROPERTIES
        BUNDLE true
        BUNDLE_EXTENSION "vst3"
        XCODE_ATTRIBUTE_WRAPPER_EXTENSION "vst3"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/mac/Info.plist"
        MACOSX_BUNDLE_BUNDLE_NAME "FogPad"
        MACOSX_BUNDLE_GUI_IDENTIFIER "nl.igorski.fogpad"
        MACOSX_BUNDLE_ICON_FILE ""
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
        MACOSX_BUNDLE_COPYRIGHT "igorski.nl © 2019"
    )
ENDIF (APPLE)

# Resources

smtg_add_vst3_resource(${target} "resource/fogpad.uidesc")
smtg_add_vst3_resource(${target} "resource/background.png")

if(MAC)
    smtg_set_bundle(${target} INFOPLIST "${CMAKE_CURRENT_SOURCE_DIR}/mac/Info.plist" PREPROCESS)
elseif(WIN)
    target_sources(${target} PRIVATE resource/fogpad.rc)
endif()

if (SMTG_CREATE_VST2_VERSION)
    message(STATUS "SMTG_CREATE_VST2_VERSION is set. A VST 2 version of the plug-in will be created (just rename the generated file from .vst3 to .vst).")
    if(XCODE)
        # fix missing VSTPluginMain symbol when also building VST 2 version
        set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_EXPORTED_SYMBOLS_FILE "")
    endif()
    if (WIN)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    endif()
endif()

endif()

######################
# Installation paths #
######################

if(APPLE)
  install(TARGETS fogpad
    DESTINATION "$ENV{HOME}/Library/Audio/Plug-Ins/VST"
  )
elseif(WIN32)
  install(TARGETS fogpad
    DESTINATION "C:/Program Files (x86)/Common Files/VST3/"
  )
elseif(WIN)
  install(TARGETS fogpad
    DESTINATION "C:/Program Files/Common Files/VST3/"
  )
elseif(UNIX AND NOT APPLE) #Linux
  install(TARGETS fogpad
    DESTINATION "/usr/lib/lxvst"
  )
endif()
