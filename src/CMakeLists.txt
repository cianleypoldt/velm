add_library(velm
            hdf5.cpp
            window.cpp
            )

target_include_directories(velm PRIVATE  .)
target_include_directories(velm PUBLIC ../include)
target_compile_features(velm PRIVATE cxx_std_17)

find_package(HDF5 REQUIRED COMPONENTS CXX)

target_link_libraries(velm
    PRIVATE
        bgfx
        bimg
        bx
        HDF5::HDF5
)

if(VELM_ENABLE_WINDOWING)
    find_package(glfw3 QUIET)

    # Resolve a usable target name from common variants so we avoid hard -lglfw3.
    set(GLFW_TARGET "")

    if (glfw3_FOUND)
        # Try the canonical imported target names in order of common usage
        if (TARGET glfw3::glfw)
            set(GLFW_TARGET glfw3::glfw)
        elseif (TARGET glfw)
            set(GLFW_TARGET glfw)
        elseif (TARGET GLFW::GLFW)
            set(GLFW_TARGET GLFW::GLFW)
        endif()
    endif()

    if (NOT GLFW_TARGET)
        # If the system package wasn't found, fetch GLFW and use the target it defines.
        include(FetchContent)
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG 3.3.8
        )
        FetchContent_MakeAvailable(glfw)

        # The GLFW project defines the target 'glfw' when built from source in most setups.
        if (TARGET glfw)
            set(GLFW_TARGET glfw)
        endif()
    endif()

    if (GLFW_TARGET)
        target_link_libraries(velm PRIVATE ${GLFW_TARGET})
    else()
        message(WARNING "GLFW not found; windowing support disabled for velm target")
    endif()
endif()
