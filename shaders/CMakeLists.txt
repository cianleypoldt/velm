# Integrate shader compilation with bgfx's shaderc (via external/bgfx.cmake)
#
# This CMakeLists will:
# - Locate .sc shader sources in this directory (vs_*.sc, fs_*.sc, cs_*.sc)
# - Pick a varying.def.sc if present here, otherwise fall back to the bgfx-provided one
# - Call bgfx_compile_shaders(...) for each shader type found
# - Create a `velm_shaders` target that depends on the compiled shader outputs
# - Expose `VELM_SHADER_INCLUDE_DIR` (path to generated headers) for downstream use
#
# Usage:
# - Put shader sources in this directory (name vertex shaders `vs_*.sc`, fragments `fs_*.sc`, computes `cs_*.sc`)
# - Optionally add a `varying.def.sc` to control varyings
# - The compiled outputs (and generated headers if enabled) will be written into:
#     ${CMAKE_BINARY_DIR}/include/generated/shaders
#
# Notes:
# - Requires the bgfx cmake helper functions (bgfx_compile_shaders) which are provided
#   by the `external/bgfx.cmake` subtree included by the top-level project.
# - The default behavior is to also produce C headers (AS_HEADERS ON). Override with
#   -DVELM_SHADERS_AS_HEADERS=OFF on the cmake command line if you do not want headers.

cmake_minimum_required(VERSION 3.14)

# Allow overriding from the top-level project or the user
if(NOT DEFINED VELM_SHADERS_AS_HEADERS)
    option(VELM_SHADERS_AS_HEADERS "Produce compiled shader headers (.bin.h) in addition to binary outputs" ON)
endif()

# Output directory for compiled shaders and headers
if(NOT DEFINED VELM_SHADERS_OUTPUT_DIR)
    set(VELM_SHADERS_OUTPUT_DIR "${CMAKE_BINARY_DIR}/include/generated/shaders" CACHE PATH "Directory where compiled shaders will be placed")
endif()

# Expose include dir variable for other targets (useful when AS_HEADERS is ON)
set(VELM_SHADER_INCLUDE_DIR "${VELM_SHADERS_OUTPUT_DIR}" CACHE PATH "Include directory with generated shader headers")

# Helper: locate varying.def.sc
set(VELM_VARYING_DEF "")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/varying.def.sc")
    set(VELM_VARYING_DEF "${CMAKE_CURRENT_SOURCE_DIR}/varying.def.sc")
elseif(EXISTS "${CMAKE_SOURCE_DIR}/varying.def.sc")
    # project-level override
    set(VELM_VARYING_DEF "${CMAKE_SOURCE_DIR}/varying.def.sc")
else()
    # fallback to bgfx's provided default if available in external/bgfx.cmake
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../external/bgfx.cmake/bgfx/src/varying.def.sc")
        set(VELM_VARYING_DEF "${CMAKE_CURRENT_SOURCE_DIR}/../external/bgfx.cmake/bgfx/src/varying.def.sc")
    elseif(EXISTS "${CMAKE_SOURCE_DIR}/external/bgfx.cmake/bgfx/src/varying.def.sc")
        set(VELM_VARYING_DEF "${CMAKE_SOURCE_DIR}/external/bgfx.cmake/bgfx/src/varying.def.sc")
    endif()
endif()

if(VELM_VARYING_DEF STREQUAL "")
    message(WARNING "shaders: No varying.def.sc found in shaders/, project root, or bgfx sources. Shader compilation may fail if varyings are required.")
else()
    message(STATUS "shaders: Using varying.def.sc -> ${VELM_VARYING_DEF}")
endif()

# Collect shader source files by convention: vs_*.sc, fs_*.sc, cs_*.sc
file(GLOB VELM_VERTEX_SHADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "vs_*.sc" "vs.*.sc")
file(GLOB VELM_FRAGMENT_SHADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "fs_*.sc" "fs.*.sc")
file(GLOB VELM_COMPUTE_SHADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "cs_*.sc" "cs.*.sc")

# Also allow any *.sc to be compiled if they don't fit conventional prefixes.
file(GLOB VELM_ALL_SC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.sc")

# Remove duplicates (if any)
list(REMOVE_DUPLICATES VELM_VERTEX_SHADERS)
list(REMOVE_DUPLICATES VELM_FRAGMENT_SHADERS)
list(REMOVE_DUPLICATES VELM_COMPUTE_SHADERS)

# If no shaders named by convention were found, try to infer by extension or default all to vertex (conservative)
if(VELM_VERTEX_SHADERS STREQUAL "" AND VELM_FRAGMENT_SHADERS STREQUAL "" AND VELM_COMPUTE_SHADERS STREQUAL "" AND VELM_ALL_SC)
    # As a safe default put every .sc into the vertex list if there is no better hint.
    set(VELM_VERTEX_SHADERS ${VELM_ALL_SC})
endif()

# Prepare to call bgfx_compile_shaders for each group if bgfx helper is available
# The bgfx helper function `bgfx_compile_shaders` is defined when external/bgfx.cmake is included.
if(COMMAND bgfx_compile_shaders)
    set(SHADER_OUTS "")
    if(VELM_VERTEX_SHADERS)
        message(STATUS "shaders: Scheduling vertex shader compilation: ${VELM_VERTEX_SHADERS}")
        bgfx_compile_shaders(
            TYPE VERTEX
            SHADERS ${VELM_VERTEX_SHADERS}
            VARYING_DEF ${VELM_VARYING_DEF}
            OUTPUT_DIR ${VELM_SHADERS_OUTPUT_DIR}
            INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
            AS_HEADERS $<BOOL:${VELM_SHADERS_AS_HEADERS}>
            OUT_FILES_VAR VELM_VERTEX_OUTS
        )
        list(APPEND SHADER_OUTS ${VELM_VERTEX_OUTS})
    endif()

    if(VELM_FRAGMENT_SHADERS)
        message(STATUS "shaders: Scheduling fragment shader compilation: ${VELM_FRAGMENT_SHADERS}")
        bgfx_compile_shaders(
            TYPE FRAGMENT
            SHADERS ${VELM_FRAGMENT_SHADERS}
            VARYING_DEF ${VELM_VARYING_DEF}
            OUTPUT_DIR ${VELM_SHADERS_OUTPUT_DIR}
            INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
            AS_HEADERS $<BOOL:${VELM_SHADERS_AS_HEADERS}>
            OUT_FILES_VAR VELM_FRAGMENT_OUTS
        )
        list(APPEND SHADER_OUTS ${VELM_FRAGMENT_OUTS})
    endif()

    if(VELM_COMPUTE_SHADERS)
        message(STATUS "shaders: Scheduling compute shader compilation: ${VELM_COMPUTE_SHADERS}")
        bgfx_compile_shaders(
            TYPE COMPUTE
            SHADERS ${VELM_COMPUTE_SHADERS}
            VARYING_DEF ${VELM_VARYING_DEF}
            OUTPUT_DIR ${VELM_SHADERS_OUTPUT_DIR}
            INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
            AS_HEADERS $<BOOL:${VELM_SHADERS_AS_HEADERS}>
            OUT_FILES_VAR VELM_COMPUTE_OUTS
        )
        list(APPEND SHADER_OUTS ${VELM_COMPUTE_OUTS})
    endif()

    # Create a convenient target that other projects can depend on
    if(SHADER_OUTS)
        add_custom_target(velm_shaders DEPENDS ${SHADER_OUTS})
    else()
        add_custom_target(velm_shaders)
    endif()

    # If the bgfx `tools` custom target exists (created when BGFX_CUSTOM_TARGETS is ON),
    # make sure our shaders are part of it so top-level builds of tools include shader build.
    if(TARGET tools)
        add_dependencies(tools velm_shaders)
    endif()

    # Expose the include dir for consumers (project code can use the VELM_SHADER_INCLUDE_DIR cache variable)
    # Optionally, if this is a subproject, add it to the global include dir cache so users can easily add:
    #   target_include_directories(myTarget PRIVATE ${VELM_SHADER_INCLUDE_DIR})
    set(VELM_SHADER_INCLUDE_DIR "${VELM_SHADERS_OUTPUT_DIR}" CACHE PATH "Directory where shader headers are generated")
else()
    message(WARNING "shaders: bgfx helper function `bgfx_compile_shaders` not available. Ensure external/bgfx.cmake is added before this directory.")
    # Provide a placeholder target so downstream CMake lists don't break when this dir is added.
    add_custom_target(velm_shaders)
endif()

# Helpful summary
message(STATUS "shaders: output -> ${VELM_SHADERS_OUTPUT_DIR}")
message(STATUS "shaders: headers enabled -> ${VELM_SHADERS_AS_HEADERS}")
