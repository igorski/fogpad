
include(SMTG_AddVST3AuV3)

function(create_audio_unit vst3_target)
    set(audio_unit_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/mac/audio-unit)
    
    set(au_app_sources
        ${audio_unit_src_dir}/Sources/ViewController.m
        ${audio_unit_src_dir}/Sources/ViewController.h
        ${audio_unit_src_dir}/Sources/AppDelegate.m
        ${audio_unit_src_dir}/Sources/AppDelegate.h
        ${audio_unit_src_dir}/audiounitconfig.h
    )

    set(au_app_ui_resources
        ${audio_unit_src_dir}/Resources/Base.lproj/Main.storyboard
        ${audio_unit_src_dir}/Resources/plugin.icns
        ${audio_unit_src_dir}/Shared/drumLoop.wav
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
    # --- E.O. SMTG_*.cmake overrides

    set(au_target ${target}_auv3)

    smtg_add_auv3_app(
        ${au_target}
        "macOS"
        "Fogpad AUV3"
        "nl.igorski.vst.${target}"
        "${audio_unit_src_dir}/audiounitconfig.h"
        "${audio_unit_src_dir}/plugin.entitlements"
        "${au_app_sources}"
        "${au_app_ui_resources}"
        "${audio_unit_src_dir}/Resources/Info.plist"
        "${audio_unit_src_dir}/Shared/Info.plist"
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
endfunction()