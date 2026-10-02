cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT DEFINED CMAKE_BUILD_TYPE AND NOT DEFINED ENV{CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Build type")
endif ()

project(tp_classic_modern_controller_ui VERSION 1.2.0 LANGUAGES CXX)

# Internal calibration UI. Public/release builds leave this OFF so users get
# the authored layout while developers can still expose every positioning
# control locally with -DTP_CLASSIC_DEV_OPTIONS=ON.
option(TP_CLASSIC_DEV_OPTIONS "Enable internal HUD calibration/editor controls" OFF)

# Pin the same Dusklight SDK revision used and tested during development.
# The official mod-template workflow will compile this source for every supported platform.
if (NOT DUSKLIGHT_VERSION)
    set(DUSKLIGHT_VERSION "d281c34a65568ee7be2319985f3a290c0ef65abc")
endif ()

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchDusklight.cmake")
add_subdirectory("${DUSKLIGHT_DIR}/sdk" dusklight-sdk EXCLUDE_FROM_ALL)

configure_file(mod.json.in "${CMAKE_CURRENT_BINARY_DIR}/mod.json" @ONLY NEWLINE_STYLE LF)

# Switch controller artwork is kept as a compact source archive. Extract it at
# configure time so add_mod packages it exactly like the existing PS/Xbox BTIs.
set(SWITCH_ASSET_ARCHIVE "${CMAKE_CURRENT_SOURCE_DIR}/assets/switch_bti.zip")
if (EXISTS "${SWITCH_ASSET_ARCHIVE}")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/res/switch")
    file(ARCHIVE_EXTRACT INPUT "${SWITCH_ASSET_ARCHIVE}" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/res/switch")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/res/switch/r1.bti"
                   "${CMAKE_CURRENT_SOURCE_DIR}/res/switch/r1_hud.bti" COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/res/switch/skill_l3.bti"
                   "${CMAKE_CURRENT_SOURCE_DIR}/res/switch/shop_l3_right.bti" COPYONLY)
endif ()

add_mod(tp_classic_modern_controller_ui
    FEATURES game
    SOURCES src/mod.cpp
    MOD_JSON "${CMAKE_CURRENT_BINARY_DIR}/mod.json"
    RES_DIR res
)

if (TP_CLASSIC_DEV_OPTIONS)
    target_compile_definitions(tp_classic_modern_controller_ui PRIVATE TP_CLASSIC_DEV_OPTIONS=1)
endif ()
