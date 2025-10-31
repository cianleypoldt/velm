add_library(velm hdf5.cpp hdf5.cpp scene.cpp)

target_include_directories(velm PRIVATE  .)
target_include_directories(velm PUBLIC ../include)

if(DEFINED VELM_SHADER_INCLUDE_DIR)
    target_include_directories(velm PUBLIC ${VELM_SHADER_INCLUDE_DIR})

    target_compile_definitions(velm PUBLIC VELM_SHADER_HEADERS_AVAILABLE=1)
endif()

target_compile_features(velm PRIVATE cxx_std_17)

if(TARGET velm_shaders)
    add_dependencies(velm velm_shaders)
endif()

if(VELM_ENABLE_HDF5)
    find_package(HDF5 REQUIRED COMPONENTS C CXX)
    target_link_libraries(velm PRIVATE ${HDF5_LIBRARIES} ${HDF5_CXX_LIBRARIES})
    target_include_directories(velm PRIVATE ${HDF5_INCLUDE_DIRS})
endif()

# Find GLM
find_package(glm REQUIRED)

# Link GLM - handle both system package and fetched content cases
if(TARGET glm::glm)
    target_link_libraries(velm PUBLIC glm::glm)
elseif(TARGET glm)
    target_link_libraries(velm PUBLIC glm)
else()
    message(FATAL_ERROR "GLM not found and unable to fetch from source")
endif()

target_link_libraries(velm
    PUBLIC
        bgfx
        bimg
        bx
)

if(VELM_ENABLE_WINDOWING)
    find_package(glfw QUIET)

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
        target_link_libraries(velm PUBLIC ${GLFW_TARGET})
    else()
        message(WARNING "GLFW not found; windowing support disabled for velm target")
    endif()
endif()
