cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT DEFINED CMAKE_BUILD_TYPE AND NOT DEFINED ENV{CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type")
endif ()

project(tp_classic_modern_controller_ui VERSION 1.3.2 LANGUAGES CXX)

# Internal calibration UI. Public/release builds leave this OFF so users get
# the authored layout while developers can still expose every positioning
# control locally with -DTP_CLASSIC_DEV_OPTIONS=ON.
option(TP_CLASSIC_DEV_OPTIONS "Enable internal HUD calibration/editor controls" OFF)

# Pin the same Dusklight SDK revision used and tested during development.
# The official mod-template workflow will compile this source for every supported platform.
if (NOT DUSKLIGHT_VERSION)
    set(DUSKLIGHT_VERSION "2d70c24b377f62cde01c6bddfb64791940d5664d")
endif ()

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchDusklight.cmake")
add_subdirectory("${DUSKLIGHT_DIR}/sdk" dusklight-sdk EXCLUDE_FROM_ALL)

configure_file(mod.json.in "${CMAKE_CURRENT_BINARY_DIR}/mod.json" @ONLY NEWLINE_STYLE LF)

# Switch/Retroid-family artwork is stored as small per-texture archives so the
# binary assets can be versioned safely. Extract them into the normal mod
# resource tree at configure time; the runtime only sees res/switch/*.bti.
set(SWITCH_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res/switch")
file(MAKE_DIRECTORY "${SWITCH_RESOURCE_DIR}")
file(GLOB SWITCH_TEXTURE_ARCHIVES
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/switch/*.tar.gz")
list(SORT SWITCH_TEXTURE_ARCHIVES)
foreach(_switch_archive IN LISTS SWITCH_TEXTURE_ARCHIVES)
    file(ARCHIVE_EXTRACT
        INPUT "${_switch_archive}"
        DESTINATION "${SWITCH_RESOURCE_DIR}")
endforeach()

add_mod(tp_classic_modern_controller_ui
    FEATURES game
    SOURCES src/mod.cpp
    MOD_JSON "${CMAKE_CURRENT_BINARY_DIR}/mod.json"
    RES_DIR res
)

if (TP_CLASSIC_DEV_OPTIONS)
    target_compile_definitions(tp_classic_modern_controller_ui PRIVATE TP_CLASSIC_DEV_OPTIONS=1)
endif ()
