# Include project headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Try to find GoogleTest in the system first
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    message(STATUS "System GoogleTest not found, fetching from source")
    # Fetch GoogleTest
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
    )
    # Prevent GoogleTest from installing files
    set(INSTALL_GTEST OFF CACHE BOOL "Disable installation of googletest" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Silence warnings in GoogleTest for GNU compilers
    if(CMAKE_COMPILER_IS_GNUCXX)
        target_compile_options(gtest PRIVATE "-w")
        target_compile_options(gtest_main PRIVATE "-w")
    endif()

    set(GTEST_LIBRARIES gtest gtest_main)
else()
    message(STATUS "Using system GoogleTest")
    set(GTEST_LIBRARIES GTest::GTest GTest::Main)
endif()

# Collect test sources
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

# Define test executable
add_executable(velm_tests ${TEST_SOURCES})

# Link with GoogleTest and project library (link the interface library as a user would)
# Link directly to the project target (no namespace) so that the target created in the
# src subdirectory is used during in-tree builds.
target_link_libraries(velm_tests PRIVATE
    velm
    ${GTEST_LIBRARIES}
)

# Add pthread only on POSIX systems
if(UNIX AND NOT APPLE) # Linux or other Unix
    # Prefer modern imported Threads target where available
    find_package(Threads REQUIRED)
    target_link_libraries(velm_tests PRIVATE Threads::Threads)
endif()

# Set output directory for test binaries
set_target_properties(velm_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
