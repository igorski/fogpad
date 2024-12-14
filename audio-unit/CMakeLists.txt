cmake_minimum_required(VERSION 3.15.0)
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17")
       set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
set(SMTG_CREATE_MODULE_INFO false)
list(APPEND CMAKE_MODULE_PATH "${VST3_SDK_ROOT}/cmake/modules")
include(SMTG_Global)
include(SMTG_AddVST3Library)
include(SMTG_Bundle)
include(SMTG_ExportedSymbols)
include(SMTG_PrefixHeader)
include(SMTG_PlatformIOS)
include(SMTG_PlatformToolset)
include(SMTG_CoreAudioSupport)
# include(SMTG_AAXSupport)
include(SMTG_VstGuiSupport)
include(SMTG_UniversalBinary)
include(SMTG_AddVST3Options)

SET(XCODE true)
SET(public_sdk_SOURCE_DIR "${VST3_SDK_ROOT}/public.sdk")
set(SMTG_VSTGUI_SOURCE_DIR "${VST3_SDK_ROOT}/vstgui4")
include(SMTG_AddVST3AuV3)

project(smtg-vst3-fogpad
    VERSION 1.0.3.0
    DESCRIPTION "igorski.nl Fogpad"
)

# if(NOT SMTG_ENABLE_VSTGUI_SUPPORT)
#     return()
# endif()

set(basepath ../)

# if(SMTG_MAC)
    # if(XCODE)
        set(fogpad_sources
            ${basepath}src/global.h
            ${basepath}src/allpass.h
            ${basepath}src/allpass.cpp
            ${basepath}src/audiobuffer.h
            ${basepath}src/audiobuffer.cpp
            ${basepath}src/bitcrusher.h
            ${basepath}src/bitcrusher.cpp
            ${basepath}src/comb.h
            ${basepath}src/comb.cpp
            ${basepath}src/decimator.h
            ${basepath}src/decimator.cpp
            ${basepath}src/filter.h
            ${basepath}src/filter.cpp
            ${basepath}src/lfo.h
            ${basepath}src/lfo.cpp
            ${basepath}src/limiter.h
            ${basepath}src/limiter.cpp
            ${basepath}src/paramids.h
            ${basepath}src/reverbprocess.h
            ${basepath}src/reverbprocess.cpp
            ${basepath}src/vst.h
            ${basepath}src/vst.cpp
            ${basepath}src/vstentry.cpp
            ${basepath}src/version.h
            ${basepath}src/ui/controller.h
            ${basepath}src/ui/controller.cpp
            ${basepath}src/ui/uimessagecontroller.h
            ${basepath}resource/plugin.uidesc

            # can we remove this
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

        set(fogpad_app_ui_resources
            "Resources/Base.lproj/Main.storyboard"
            "Resources/plugin.icns"
        )

        set(target fogpad)

        smtg_add_vst3plugin(${target} ${fogpad_sources})
        smtg_target_configure_version_file(${target})

        target_compile_features(${target}
            PUBLIC
                cxx_std_17
        )

        target_include_directories(${target}
            PUBLIC
                ${SMTG_VSTGUI_SOURCE_DIR}
        )

        # target_link_libraries(${target}
        #     PRIVATE
        #         base_ios
        #         sdk_ios
        #         "-framework UIKit"
        #         "-framework CoreGraphics" 
        #         "-framework QuartzCore" 
        #         "-framework CoreText" 
        #         "-framework Accelerate" 
        #         "-framework ImageIO" 
        #         "-framework MobileCoreServices"
        # )
        
        set(steinberg_libs "base" "pluginterfaces" "sdk" "sdk_hosting" "vstgui" "vstgui_support" "vstgui_uidescription")
        include_directories(${VST3_SDK_ROOT})
        link_directories("${VST3_SDK_ROOT}/build/lib")
        
        foreach(lib IN ITEMS ${steinberg_libs})
            target_link_libraries(${target} PRIVATE ${VST3_SDK_ROOT}/build/lib/Release/lib${lib}.a)
        endforeach(lib)

        target_sources(${target} 
            PRIVATE 
                ${SMTG_VSTGUI_SOURCE_DIR}/vstgui/vstgui_uidescription.cpp
                # ${SMTG_VSTGUI_SOURCE_DIR}/vstgui/vstgui_ios.mm
                ${SMTG_VSTGUI_SOURCE_DIR}/vstgui/plugin-bindings/vst3editor.cpp
                ${VST3_SDK_ROOT}/public.sdk/source/vst/vstguieditor.cpp
        )

        find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
        find_library(COCOA_FRAMEWORK Cocoa)
        find_library(OPENGL_FRAMEWORK OpenGL)
        find_library(ACCELERATE_FRAMEWORK Accelerate)
        find_library(QUARTZCORE_FRAMEWORK QuartzCore)
        find_library(CARBON_FRAMEWORK Carbon)
        find_library(EXPAT Expat)
        target_link_libraries(${target} PRIVATE ${COREFOUNDATION_FRAMEWORK} ${COCOA_FRAMEWORK} ${OPENGL_FRAMEWORK} ${ACCELERATE_FRAMEWORK} ${QUARTZCORE_FRAMEWORK} ${CARBON_FRAMEWORK} ${EXPAT})

        
        #--------------------------------------------------------------------------------------------------------
        # macOS target
        #--------------------------------------------------------------------------------------------------------
        
        smtg_add_auv3_app(
            fogpad_auv3_macos
            "macOS"
            "Fogpad AUV3 macOS"
            "nl.igorski.vst.${target}.audiounit"
            "audiounitconfig.h"
            "plugin.entitlements"
            "${fogpad_sources}"
            "${fogpad_app_ui_resources}"
            "Info.plist"
            "Shared/Info.plist"
            ${target}
        )
    # endif(XCODE)
# endif(SMTG_MAC)