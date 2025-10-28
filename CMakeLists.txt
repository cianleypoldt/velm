cmake_minimum_required(VERSION 3.14)
project("velm" CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
message(STATUS "CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE})

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(VELM_TOP_LEVEL_PROJECT TRUE)
    message(STATUS "Building as top level project")
else()
    set(VELM_TOP_LEVEL_PROJECT FALSE)
    message(STATUS "Building as subdirectory")
endif()

option(VELM_ENABLE_WINDOWING "Enable optional windowing integration (GLFW)" ON)
option(VELM_ENABLE_HDF5 "Enable optional HDF5 support" ON)
option(VELM_BUILD_DEV "Enable developer utilities" ON)
option(VELM_ENABLE_SANITIZER "Enable address and undefined behavior sanitizer" OFF)
option(VELM_ENABLE_COVERAGE "Enable code coverage reporting" OFF)

message(STATUS "VELM_ENABLE_WINDOWING: ${VELM_ENABLE_WINDOWING}")
message(STATUS "VELM_ENABLE_HDF5: ${VELM_ENABLE_HDF5}")
message(STATUS "VELM_BUILD_DEV: ${VELM_BUILD_DEV}")
message(STATUS "VELM_ENABLE_SANITIZER: ${VELM_ENABLE_SANITIZER}")
message(STATUS "VELM_ENABLE_COVERAGE: ${VELM_ENABLE_COVERAGE}")

if(VELM_ENABLE_SANITIZER)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -fno-omit-frame-pointer")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined")
        message(STATUS "Enabled sanitizer flags for GCC/Clang")
    elseif(MSVC)
        message(WARNING "Sanitizer options are not supported with MSVC")
    endif()
endif()

if(VELM_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    else()
        message(WARNING "Coverage options are only supported with GCC and Clang")
    endif()
endif()

if(VELM_ENABLE_HDF5)
    add_definitions(-DVELM_ENABLE_HDF5)
endif()
if(VELM_ENABLE_WINDOWING)
    add_definitions(-DVELM_ENABLE_WINDOWING)
endif()

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/external/bgfx.cmake")

if(APPLE)
    target_compile_options(bgfx PRIVATE
    -Wno-error
    )
    target_compile_options(bimg_decode PRIVATE
    -Wno-error
    )
endif()

add_subdirectory(src)

include(CTest)
if(BUILD_TESTING)
    message(STATUS "Building tests")
    enable_testing()
    add_subdirectory(tests)
endif()

if(VELM_BUILD_DEV)
    message(STATUS "Building developer utilities")
    add_subdirectory(dev)
endif()
