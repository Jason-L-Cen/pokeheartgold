cmake_minimum_required(VERSION 3.20)
project(HeartGoldOriginalData LANGUAGES C)
get_filename_component(original_root "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(generated "${original_root}/ios/build-original-host/include")
if(NOT EXISTS "${generated}/msgdata/msg.naix")
    message(FATAL_ERROR "Run python3 ios/tools/prepare_original_headers.py before configuring the original core")
endif()

set(original_sources
    pokemon.c math_util.c filesystem.c seal_case.c pm_string.c item.c move.c
    msgdata.c map_section.c mail.c mail_message.c string_util.c pm_version.c orbit_cursor.c)
list(TRANSFORM original_sources PREPEND "${original_root}/src/")
add_library(HeartGoldOriginalData STATIC ${original_sources} NativeMemory.c NativeFile.c NativeFndData.c
    "${original_root}/lib/NitroSystem/src/fnd_expheap.c"
    "${original_root}/lib/NitroSystem/src/fnd_heapcommon.c"
    "${original_root}/lib/NitroSystem/src/fnd_list.c"
    "${original_root}/lib/NitroSystem/src/fnd_allocator.c")
target_compile_features(HeartGoldOriginalData PUBLIC c_std_11)
target_compile_definitions(HeartGoldOriginalData PUBLIC
    HEARTGOLD_NATIVE SDK_ARM9 SDK_CW SDK_CODE_ARM SDK_TS _NITRO
    PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz PM_KEEP_ASSERTS)
target_include_directories(HeartGoldOriginalData PUBLIC
    "${generated}" "${original_root}/include" "${original_root}/files"
    "${original_root}/lib/include" "${original_root}/lib/include/cw"
    "${original_root}/lib/include/nitro" "${original_root}/lib/include/nnsys"
    "${original_root}/lib/include/nitro/os" "${original_root}/include/library"
    "${original_root}/lib/NitroSDK/include")
target_compile_options(HeartGoldOriginalData PUBLIC
    -fwrapv -fno-strict-aliasing -fms-extensions -Wno-microsoft -Wno-unknown-pragmas
    -ffunction-sections -fdata-sections)
set_target_properties(HeartGoldOriginalData PROPERTIES
    XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH NO XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED NO)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
    enable_testing()
    add_executable(OriginalDataCheck tests/abi_check.c)
    target_link_libraries(OriginalDataCheck PRIVATE HeartGoldOriginalData)
    target_compile_options(HeartGoldOriginalData PRIVATE -fsanitize=address,undefined -fno-sanitize-recover=all -g)
    target_compile_options(OriginalDataCheck PRIVATE -fsanitize=address,undefined -fno-sanitize-recover=all -g)
    target_link_options(OriginalDataCheck PRIVATE -fsanitize=address,undefined -Wl,-dead_strip)
    add_test(NAME OriginalDataCheck COMMAND OriginalDataCheck
        "${original_root}/ios/build-original-host/reference-content")
endif()
