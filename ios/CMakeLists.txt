cmake_minimum_required(VERSION 3.20)
project(HeartGoldIOS LANGUAGES C CXX OBJCXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(core_patch "${CMAKE_CURRENT_SOURCE_DIR}/patches/melonDS-initial-renderer.patch")
set(core_dir "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/melonDS")
execute_process(COMMAND git -C "${core_dir}" apply --reverse --check "${core_patch}"
    RESULT_VARIABLE core_patch_present OUTPUT_QUIET ERROR_QUIET)
if(NOT core_patch_present EQUAL 0)
    execute_process(COMMAND git -C "${core_dir}" apply --check "${core_patch}" RESULT_VARIABLE core_patch_valid)
    if(NOT core_patch_valid EQUAL 0)
        message(FATAL_ERROR "The pinned melonDS initialization patch cannot be applied; local changes were preserved.")
    endif()
    execute_process(COMMAND git -C "${core_dir}" apply "${core_patch}" COMMAND_ERROR_IS_FATAL ANY)
endif()

# iOS cannot depend on executable-memory entitlements or desktop graphics APIs.
set(ENABLE_JIT OFF CACHE BOOL "" FORCE)
set(ENABLE_OGLRENDERER OFF CACHE BOOL "" FORCE)
set(ENABLE_GDBSTUB OFF CACHE BOOL "" FORCE)
set(BUILD_QT_SDL OFF CACHE BOOL "" FORCE)
set(ENABLE_LTO_RELEASE OFF CACHE BOOL "" FORCE)
add_subdirectory(Vendor/melonDS EXCLUDE_FROM_ALL)

add_library(heartgold-host STATIC App/Platform.cpp)
target_link_libraries(heartgold-host PUBLIC core)
target_include_directories(heartgold-host PUBLIC App)

if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(art "${CMAKE_CURRENT_SOURCE_DIR}/../files/poketool/pokegra/pokegra")
    set(images)
    foreach(species 0152 0155 0158)
        configure_file("${art}/${species}/male/front.png" "${CMAKE_CURRENT_BINARY_DIR}/${species}.png" COPYONLY)
        list(APPEND images "${CMAKE_CURRENT_BINARY_DIR}/${species}.png")
    endforeach()
    configure_file("${art}/0155/male/back.png" "${CMAKE_CURRENT_BINARY_DIR}/0155-back.png" COPYONLY)
    list(APPEND images "${CMAKE_CURRENT_BINARY_DIR}/0155-back.png")
    set_source_files_properties(${images} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    add_executable(HeartGoldIOS MACOSX_BUNDLE App/main.mm ${images})
    target_compile_options(HeartGoldIOS PRIVATE -fobjc-arc)
    target_link_libraries(HeartGoldIOS PRIVATE heartgold-host "-framework UIKit" "-framework CoreGraphics" "-framework QuartzCore" "-framework AudioToolbox" "-framework UniformTypeIdentifiers")
    set_target_properties(HeartGoldIOS PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.jasoncen.heartgold"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.jasoncen.heartgold"
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
        XCODE_ATTRIBUTE_SUPPORTS_MACCATALYST NO
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)
else()
    enable_testing()
    add_executable(heartgold-check tests/check.cpp)
    target_link_libraries(heartgold-check PRIVATE heartgold-host)
    target_compile_options(heartgold-check PRIVATE -UNDEBUG)
    add_test(NAME heartgold-check COMMAND heartgold-check)
endif()
