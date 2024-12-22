
include(SMTG_AddVST3AuV3)

function(create_audio_unit vst3_target)
    message(STATUS "SMTG_CREATE_AU_VERSION is set. Creating Audio Unit version of the plugin.")
    
    set(au_app_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/audio-unit/Sources/ViewController.m
        ${CMAKE_CURRENT_SOURCE_DIR}/audio-unit/Sources/ViewController.h
        ${CMAKE_CURRENT_SOURCE_DIR}/audio-unit/Sources/AppDelegate.m
        ${CMAKE_CURRENT_SOURCE_DIR}/audio-unit/Sources/AppDelegate.h
        ${CMAKE_CURRENT_SOURCE_DIR}/audio-unit/audiounitconfig.h
    )

    set(au_app_ui_resources
        ${CMAKE_CURRENT_SOURCE_DIR}/audio-unit/Resources/Base.lproj/Main.storyboard
        ${CMAKE_CURRENT_SOURCE_DIR}/audio-unit/Resources/plugin.icns
        ${CMAKE_CURRENT_SOURCE_DIR}/audio-unit/Shared/drumLoop.wav
    )
    
    # these redefine values set in SMTG_AddVST3AuV3.cmake ---
    set(public_sdk_SOURCE_DIR ${VST3_SDK_ROOT}/public.sdk)
    set(auv3wrappermacos_sources
        ${VST3_SDK_ROOT}/public.sdk/source/vst/auv3wrapper/AUv3WrappermacOS/main.mm
    )
    set(auv3wrappermacosextension_sources
        ${VST3_SDK_ROOT}/public.sdk/source/vst/auv3wrapper/Shared/AUv3WrapperFactory.mm
        ${VSTSDK_PLUGIN_SOURCE}
    )   
    # these redefine values set in SMTG_CodeSign.cmake
    set(SMTG_XCODE_OTHER_CODE_SIGNING_FLAGS "--timestamp --deep" CACHE STRING "Other code signing flags [Xcode]")
     # --- E.O. SMTG_*.cmake overrides

    set(au_target ${target}_auv3)

    smtg_add_auv3_app(
        ${au_target}
        "macOS"
        "Fogpad AUV3"
        "nl.igorski.vst.${target}"
        "${CMAKE_CURRENT_SOURCE_DIR}/audio-unit/audiounitconfig.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/audio-unit/plugin.entitlements"
        "${au_app_sources}"
        "${au_app_ui_resources}"
        "${CMAKE_CURRENT_SOURCE_DIR}/audio-unit/Resources/Info.plist"
        "${CMAKE_CURRENT_SOURCE_DIR}/audio-unit/Shared/Info.plist"
        ${vst3_target}
    )

    #exposes auv3wrappermacos
    target_link_directories(${au_target}_appextension_macos PRIVATE "${VST3_SDK_ROOT}/build/lib")
    target_link_directories(${au_target} PRIVATE "${VST3_SDK_ROOT}/build/lib")
  
    target_link_libraries(${au_target}
        PUBLIC
            base
            pluginterfaces
            sdk
            sdk_common
            sdk_hosting
    )

    target_link_libraries(${au_target}_appextension_macos
        PUBLIC
            base
            pluginterfaces
            sdk
            sdk_hosting
    )

    install(TARGETS ${au_target}
        DESTINATION "$ENV{HOME}/Library/Audio/Plug-Ins/Components"
    )
endfunction()