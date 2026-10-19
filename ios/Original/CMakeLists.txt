cmake_minimum_required(VERSION 3.20)
project(HeartGoldOriginalData LANGUAGES C CXX)
get_filename_component(original_root "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(generated "${original_root}/ios/build-original-host/include")
if(NOT EXISTS "${generated}/msgdata/msg.naix" OR NOT EXISTS "${generated}/hg_native_registers.h")
    message(FATAL_ERROR "Run python3 ios/tools/prepare_original_headers.py before configuring the original core")
endif()

set(original_sources
    pokemon.c math_util.c filesystem.c heap.c seal_case.c pm_string.c item.c move.c
    msgdata.c map_section.c mail.c mail_message.c string_util.c pm_version.c orbit_cursor.c
    sys_task.c sys_task_api.c system.c billboard_gfx_sequence.c field_effects.c gf_rtc.c bg_window.c
    save.c save_arrays.c apricorn_box.c bag.c battle_record.c daycare.c easy_chat.c friend_group.c friend_roster.c
    frontier_save.c game_stats.c hall_of_fame.c mystery_gift.c palPark_migration.c party.c photo_album.c player_data.c
    pokeathlon/pokeathlon_save.c pokedex.c pokemon_storage_system.c pokewalker.c roamer.c safari_zone.c sav_chatot.c
    sav_system_info.c save_block_23.c save_fashion.c save_follow_mon.c save_gts.c save_link_ruleset.c save_local_field_data.c
    save_misc.c save_palpad.c save_plaza_survey.c save_pokegear.c save_rankings.c save_special_ribbons.c
    save_trainer_card.c save_trainer_house.c save_vars_flags.c save_wifi_history.c save_wii_message.c
    apricorn_tree_sys.c coins.c player_avatar.c igt.c comm_state.c frontier_data.c save_gymmick.c frontier_state.c options.c
    spawn_warp.c save_data_read_error.c save_data_write_error.c gf_gfx_loader.c sprite_resource.c)
list(TRANSFORM original_sources PREPEND "${original_root}/src/")
add_library(HeartGoldOriginalData STATIC ${original_sources} NativeMemory.c NativeFile.c NativeBackup.c NativeFndData.c NativeSaveData.c NativeDwcIdentity.c NativeOSMemory.c NativeCPU.c NativeLockIDs.c NativeIntegerRuntime.c NativeFloatRuntime.c NativeEvents.c NativeTick.c NativeDMA.c NativeRTC.c NativeGraphicsMemory.c Native2D.cpp NativeCompression.c
    "${original_root}/lib/NitroSystem/src/fnd_expheap.c"
    "${original_root}/lib/NitroSystem/src/fnd_heapcommon.c"
    "${original_root}/lib/NitroSystem/src/fnd_list.c"
    "${original_root}/lib/NitroSystem/src/fnd_allocator.c"
    "${original_root}/lib/NitroSDK/src/mi/mi_memory.c"
    "${original_root}/lib/NitroSDK/src/mi/mi_uncompress.c"
    "${original_root}/lib/NitroSDK/src/cp/CP_context.c"
    "${original_root}/lib/NitroSDK/src/fx/fx_cp.c"
    "${original_root}/lib/NitroSDK/src/fx/fx_vec.c"
    "${original_root}/lib/NitroSDK/src/fx/fx_vec_normalize.c"
    "${original_root}/lib/NitroSDK/src/fx/fx_vec_mult_add.c"
    "${original_root}/lib/NitroSDK/src/fx/fx_mtx44_perspective.c"
    "${original_root}/lib/NitroSDK/src/fx/fx_mtx44_ortho.c"
    "${original_root}/lib/NitroSDK/src/mi/mi_swap.c"
    "${original_root}/lib/NitroSDK/src/os/os_cache.c"
    NativeDgtData.c
    NativeSystemCalls.c
    NativeGeometry.c
    "${original_root}/lib/NitroSDK/src/fx/fx_mtx44_concat.c"
    "${original_root}/lib/NitroSDK/src/gx/g3imm.c"
    "${original_root}/lib/NitroSDK/src/gx/g3.c"
    "${original_root}/lib/NitroSDK/src/gx/g3b.c"
    "${original_root}/lib/NitroSDK/src/gx/g3x.c"
    "${original_root}/lib/NitroSDK/src/gx/gxasm.c"
    "${original_root}/lib/NitroSDK/src/gx/g3x_nop_clear_fifo.c"
    "${original_root}/lib/NitroSDK/src/dgt_hash.c"
    "${original_root}/lib/NitroSDK/src/dgt_hash2_arm4_small.c"
    "${original_root}/lib/libCPS/src/cps_sha1.c"
    "${original_root}/lib/libCPS/src/cps_bignum.c"
    "${original_root}/lib/NitroWiFi/src/wcm.c"
    "${original_root}/lib/NitroWiFi/src/wcm_control.c"
    "${original_root}/lib/src/nitrocrypto_rc4_init.c"
    "${original_root}/lib/src/nitrocrypto_rc4.c"
    "${original_root}/lib/src/nitrocrypto_rc4_fast.c"
    "${original_root}/lib/NitroSDK/src/rtc/rtc_convert_pre.c"
    "${original_root}/lib/NitroSDK/src/rtc/rtc_convert_post.c"
    "${original_root}/lib/NitroSDK/src/rtc/rtc_convert_date_time_to_second.c"
    "${original_root}/lib/NitroSDK/src/math/MATH_crc.c"
    "${original_root}/lib/NitroSDK/src/math/math_qsort.c"
    "${original_root}/lib/NitroDWC/src/dwc_account.c"
    "${original_root}/lib/NitroSDK/src/gx/gx.c"
    "${original_root}/lib/NitroSDK/src/gx/gxstate.c"
    "${original_root}/lib/NitroSDK/src/gx/gx_vramcnt.c"
    "${original_root}/lib/NitroSDK/src/gx/gx_load2d.c"
    "${original_root}/lib/NitroSDK/src/gx/gx_bgcnt.c"
    "${original_root}/lib/NitroSDK/src/gx/g2.c"
    "${original_root}/lib/NitroSDK/src/os/os_vramExclusive.c"
    "${original_root}/lib/NitroSDK/src/os/os_vram_exclusive_clz.c")
target_compile_features(HeartGoldOriginalData PUBLIC c_std_11 cxx_std_17)
target_include_directories(HeartGoldOriginalData PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(HeartGoldOriginalData PUBLIC
    HEARTGOLD_NATIVE SDK_ARM9 SDK_CW SDK_CODE_ARM SDK_TS _NITRO
    PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz PM_KEEP_ASSERTS)
target_include_directories(HeartGoldOriginalData PUBLIC
    "${generated}" "${original_root}/include" "${original_root}/files" "${original_root}/src"
    "${original_root}/lib/include" "${original_root}/lib/include/cw"
    "${original_root}/lib/include/nitro" "${original_root}/lib/include/nnsys"
    "${original_root}/lib/include/nitro/os" "${original_root}/include/library"
    "${original_root}/lib/NitroSDK/include" "${original_root}/lib/NitroSDK/include-pokediamond")
target_compile_options(HeartGoldOriginalData PUBLIC
    -fwrapv -fno-strict-aliasing -fms-extensions -Wno-microsoft -Wno-unknown-pragmas
    -ffunction-sections -fdata-sections)
set_target_properties(HeartGoldOriginalData PROPERTIES
    XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH NO XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED NO)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
    enable_testing()
    add_executable(OriginalDataCheck tests/abi_check.c tests/memory_check.c tests/swap_check.c tests/cache_check.c tests/cpu_check.c tests/integer_check.c tests/float_check.c tests/rc4_check.c tests/digest_check.c tests/qsort_check.c tests/bignum_check.c tests/wcm_check.c tests/runtime_check.c tests/effect_check.c tests/dma_check.c tests/rtc_check.c tests/backup_check.c tests/save_check.c tests/graphics_check.c tests/render_check.c tests/resource_check.c tests/environment.c
        "${original_root}/lib/libCPS/src/cps_rc4.c")
    target_link_libraries(OriginalDataCheck PRIVATE HeartGoldOriginalData)
    target_sources(OriginalDataCheck PRIVATE tests/system_call_check.c)
    target_sources(OriginalDataCheck PRIVATE tests/compression_check.c)
    target_sources(OriginalDataCheck PRIVATE tests/math_unit_check.c)
    target_sources(OriginalDataCheck PRIVATE tests/geometry_check.c)
    target_sources(OriginalDataCheck PRIVATE tests/primitive_check.c)
    target_sources(OriginalDataCheck PRIVATE tests/arena_check.c)
    target_sources(OriginalDataCheck PRIVATE tests/legacy_irq_check.c)
    target_sources(OriginalDataCheck PRIVATE tests/linker_binding_check.c
        "${original_root}/src/frontier_runtime.c"
        "${original_root}/lib/NitroSDK/src/snd/SND_util.c"
        "${original_root}/lib/NitroSystem/src/snd_fader.c")
    target_compile_options(HeartGoldOriginalData PRIVATE -fsanitize=address,undefined -fno-sanitize-recover=all -g)
    target_compile_options(OriginalDataCheck PRIVATE -fsanitize=address,undefined -fno-sanitize-recover=all -g)
    target_link_options(OriginalDataCheck PRIVATE -fsanitize=address,undefined -Wl,-dead_strip)
    add_test(NAME OriginalDataCheck COMMAND OriginalDataCheck
        "${original_root}/ios/build-original-host/reference-content")
endif()
