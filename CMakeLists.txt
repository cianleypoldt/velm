cmake_minimum_required(VERSION 3.19)
project(VelmVT LANGUAGES CXX C)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(VELM_OUTPUT_BIN_DIR "${CMAKE_BINARY_DIR}/bin")
set(VELM_OUTPUT_LIB_DIR "${CMAKE_BINARY_DIR}/lib")

# global output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${VELM_OUTPUT_BIN_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${VELM_OUTPUT_LIB_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${VELM_OUTPUT_LIB_DIR}")

# per-configuration output directories for multi-config generators
foreach(CONFIG IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${CONFIG}" CONFIG_UPPER)
    set("CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER}" "${VELM_OUTPUT_BIN_DIR}")
    set("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER}" "${VELM_OUTPUT_LIB_DIR}")
    set("CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER}" "${VELM_OUTPUT_LIB_DIR}")
endforeach()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(VELM_TOP_LEVEL_PROJECT TRUE)
else()
    set(VELM_TOP_LEVEL_PROJECT FALSE)
endif()

set(VELM_SOURCE_DIR CMAKE_CURRENT_SOURCE_DIR)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(VELM_ENABLE_TESTING ON)
option(VELM_BUILD_DEV ON)
option(VELM_ENABLE_WINDOWING "Enable optional windowing integration (GLFW)" ON)
option(VELM_ENABLE_HDF5 "Enable optional HDF5 support" ON)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/external/bgfx.cmake")

add_subdirectory(src)

if(VELM_ENABLE_TESTING)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()

if(VELM_BUILD_DEV)
    add_subdirectory(dev)
endif()
