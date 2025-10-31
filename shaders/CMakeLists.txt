set(VELM_SHADERS_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/shaders" CACHE PATH "Directory where compiled shaders will be placed")
file(MAKE_DIRECTORY ${VELM_SHADERS_OUTPUT_DIR})

set(VELM_VARYING_DEF "${CMAKE_CURRENT_SOURCE_DIR}/varying.def.sc")

file(GLOB VELM_VERTEX_SHADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "vs_*.sc" "vs.*.sc")
file(GLOB VELM_FRAGMENT_SHADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "fs_*.sc" "fs.*.sc")
file(GLOB VELM_COMPUTE_SHADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "cs_*.sc" "cs.*.sc")

list(REMOVE_DUPLICATES VELM_VERTEX_SHADERS)
list(REMOVE_DUPLICATES VELM_FRAGMENT_SHADERS)
list(REMOVE_DUPLICATES VELM_COMPUTE_SHADERS)

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
            OUT_FILES_VAR VELM_COMPUTE_OUTS
        )
        list(APPEND SHADER_OUTS ${VELM_COMPUTE_OUTS})
    endif()

    if(SHADER_OUTS)
        add_custom_target(velm_shaders DEPENDS ${SHADER_OUTS})
    else()
        add_custom_target(velm_shaders)
    endif()

    if(TARGET tools)
        add_dependencies(tools velm_shaders)
    endif()
    set(VELM_SHADER_INCLUDE_DIR "${VELM_SHADERS_OUTPUT_DIR}" CACHE PATH "Directory where shader binaries are generated")
else()
    message(WARNING "shaders: bgfx helper function `bgfx_compile_shaders` not available. Ensure external/bgfx.cmake is added before this directory.")
    # Provide a placeholder target so downstream CMake lists don't break when this dir is added.
    add_custom_target(velm_shaders)
endif()

message(STATUS "shaders: output -> ${VELM_SHADERS_OUTPUT_DIR}")
