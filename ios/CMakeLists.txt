cmake_minimum_required(VERSION 3.20)
project(HeartGoldNative LANGUAGES Swift)
file(GLOB resources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Resources/*")
set_source_files_properties(${resources} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/NativeAssets")
add_executable(HeartGoldNative MACOSX_BUNDLE
    App/NativeApp.swift App/Game.swift App/GameController.swift App/WorldRenderer.swift ${resources})
target_link_libraries(HeartGoldNative PRIVATE
    "-framework UIKit" "-framework MetalKit" "-framework Metal"
    "-framework QuartzCore" "-framework CoreGraphics" "-framework Foundation")
set_target_properties(HeartGoldNative PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.jasoncen.heartgold.native"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.jasoncen.heartgold.native"
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
    XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH NO
    XCODE_ATTRIBUTE_SUPPORTS_MACCATALYST NO
    XCODE_ATTRIBUTE_SWIFT_VERSION 5.0)
