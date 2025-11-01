#include "velm/shader_system.h"

#include "bgfx/bgfx.h"
#include "bx/file.h"

#include <filesystem>
#include <iostream>
#include <string_view>

void velm_shadersys::load_all() {
    const char * shaderPath = "";

    switch (bgfx::getRendererType()) {
        case bgfx::RendererType::Direct3D11:
        case bgfx::RendererType::Direct3D12:
        case bgfx::RendererType::Vulkan:
            shaderPath = "shaders/spirv/";
            break;
        case bgfx::RendererType::Metal:
            shaderPath = "shaders/metal/";
            break;
        case bgfx::RendererType::OpenGL:
        case bgfx::RendererType::OpenGLES:
            shaderPath = "shaders/glsl/";
            break;
        default:
            abort();
            return;
    };
    std::filesystem::path shader_dir(shaderPath);  // Get st path in shader list of .sc files
    std::cout << "Accessing shader directory at: " << std::filesystem::absolute(shader_dir) << std::endl;

    for (const auto & entry : std::filesystem::directory_iterator(shader_dir)) {
        if (entry.path().extension() == ".bin") {
            std::string shader_name = entry.path().stem().string();
            std::cout << "Loading shader from: " << std::filesystem::absolute(entry.path()) << std::endl;

            // Load shader file
            bx::FileReader reader;
            if (bx::open(&reader, entry.path().string().c_str())) {
                // Get file size
                int32_t size = (int32_t) bx::getSize(&reader);

                // Read entire file
                const bgfx::Memory * mem = bgfx::alloc(size + 1);
                bx::read(&reader, mem->data, size, bx::ErrorAssert{});
                bx::close(&reader);

                // Null terminate
                mem->data[mem->size - 1] = '\0';

                // Create shader
                bgfx::ShaderHandle handle = bgfx::createShader(mem);

                if (bgfx::isValid(handle)) {
                    // Add to shader collection
                    shaders.push_back(std::make_pair(shader_name, handle));
                }
            }
        }
    }
}

void velm_shadersys::destroy_all() {
    for (int i = 0; i < shaders.size(); i++) {
        std::cout << "Shader system: Destroying shader " << shaders[i].first << "\n";
        bgfx::destroy(shaders[i].second);
    }
}

bgfx::ShaderHandle velm_shadersys::retrieve(std::string_view name) {
    for (int i = 0; i < shaders.size(); i++) {
        if (shaders[i].first == name) {
            return shaders[i].second;
        }
    }
    return bgfx::ShaderHandle();
}
