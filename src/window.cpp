#include "velm/window.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <GLFW/glfw3.h>

#ifndef GLFW_EXPOSE_NATIVE_X11
#    define GLFW_EXPOSE_NATIVE_X11
#endif
#ifndef GLFW_EXPOSE_NATIVE_WAYLAND
#    define GLFW_EXPOSE_NATIVE_WAYLAND
#endif
#include <GLFW/glfw3native.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>

struct PosColorVertex {
    float    x;
    float    y;
    float    z;
    uint32_t abgr;

    static void initLayout(bgfx::VertexLayout & layout) {
        layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true, true)
            .end();
    }
};

static std::vector<uint8_t> loadFile(const char * filePath) {
    std::vector<uint8_t> mem;
    std::ifstream        ifs(filePath, std::ios::binary | std::ios::ate);
    if (!ifs) {
        return mem;
    }
    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    mem.resize(size);
    if (!ifs.read(reinterpret_cast<char *>(mem.data()), size)) {
        mem.clear();
    }
    return mem;
}

void test_namespace::open_and_close() {
    if (!glfwInit()) {
        fprintf(stderr, "glfwInit() failed\n");
        return;
    }
    GLFWwindow * window = glfwCreateWindow(1000, 500, "skibidi window", NULL, NULL);
    if (!window) {
        fprintf(stderr, "glfwCreateWindow() failed\n");
        return;
    }
    glfwShowWindow(window);

    // Create native platform data for bgfx using GLFW native functions (X11/Wayland).
    bgfx::PlatformData pd;
    pd.context      = nullptr;
    pd.backBuffer   = nullptr;
    pd.backBufferDS = nullptr;
    // Choose native handles based on environment (Wayland vs X11). Hyprland uses Wayland.
    if (getenv("WAYLAND_DISPLAY")) {
        // Wayland
        pd.ndt = glfwGetWaylandDisplay();
        pd.nwh = (void *) (uintptr_t) glfwGetWaylandWindow(window);
        fprintf(stderr, "Using Wayland native handles for bgfx platform data\n");
    } else {
        // Fallback to X11
        pd.ndt = glfwGetX11Display();
        pd.nwh = (void *) (uintptr_t) glfwGetX11Window(window);
        fprintf(stderr, "Using X11 native handles for bgfx platform data\n");
    }
    bgfx::setPlatformData(pd);

    // Initialize bgfx with auto renderer selection.
    bgfx::Init init;
    init.type              = bgfx::RendererType::Count;  // auto
    init.platformData      = pd;
    init.resolution.width  = 1000;
    init.resolution.height = 500;
    init.resolution.reset  = BGFX_RESET_VSYNC;
    if (!bgfx::init(init)) {
        // Failed to initialize bgfx; fall back to exiting.
        glfwDestroyWindow(window);
        return;
    }

    // Print selected renderer type for debugging.
    bgfx::RendererType::Enum rt = bgfx::getRendererType();
    fprintf(stderr, "bgfx initialized. Renderer: %d\n", (int) rt);

    // Define vertex layout and triangle vertex data.
    bgfx::VertexLayout layout;
    PosColorVertex::initLayout(layout);

    PosColorVertex vertices[] = {
        { 0.0f,  0.6f,  0.0f, 0xff0000ff }, // Red (ABGR)
        { -0.6f, -0.4f, 0.0f, 0xff00ff00 }, // Green
        { 0.6f,  -0.4f, 0.0f, 0xffff0000 }  // Blue
    };

    const uint16_t indices[] = { 0, 1, 2 };

    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(bgfx::makeRef(vertices, sizeof(vertices)), layout);

    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));

    // Load precompiled shader binaries (platform-specific) from files.
    // Note: The shader binaries must be produced with bgfx shaderc for the target renderer.
    std::vector<uint8_t> vs = loadFile("vs_triangle.bin");
    std::vector<uint8_t> fs = loadFile("fs_triangle.bin");
    fprintf(stderr, "vs_triangle.bin size=%zu, fs_triangle.bin size=%zu\n", vs.size(), fs.size());

    bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
    if (!vs.empty() && !fs.empty()) {
        bgfx::ShaderHandle vsh = bgfx::createShader(bgfx::copy(vs.data(), (uint32_t) vs.size()));
        bgfx::ShaderHandle fsh = bgfx::createShader(bgfx::copy(fs.data(), (uint32_t) fs.size()));
        program                = bgfx::createProgram(vsh, fsh, true /* destroy shaders when program destroyed */);
        if (bgfx::isValid(program)) {
            fprintf(stderr, "Shader program created successfully\n");
        } else {
            fprintf(stderr, "Failed to create shader program. Check shader binaries and renderer type\n");
        }
    } else {
        fprintf(stderr, "Shader binaries not found: vs=%zu fs=%zu\n", vs.size(), fs.size());
        // If shaders are not available, create a simple passthrough by touching the view so we still get clears.
        // Without proper shaders, submitting the triangle will be a no-op.
    }

    // Set view 0 clear state.
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xFF1A1F26, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, 1000, 500);

    // Main loop: render triangle using bgfx.
    while (!glfwWindowShouldClose(window)) {
        int fbw = 0;
        int fbh = 0;
        glfwGetFramebufferSize(window, &fbw, &fbh);

        if (fbw == 0 || fbh == 0) {
            // Minimized or zero-sized; just poll events.
            glfwPollEvents();
            usleep(10000);
            continue;
        }

        // Reset resolution if it changed.
        bgfx::reset((uint32_t) fbw, (uint32_t) fbh, BGFX_RESET_VSYNC);
        bgfx::setViewRect(0, 0, 0, (uint16_t) fbw, (uint16_t) fbh);

        // Set view and submit draw call.
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xFF1A1F26, 1.0f, 0);
        bgfx::touch(0);  // ensure view 0 is cleared even if nothing submitted

        if (bgfx::isValid(program)) {
            float mtx[16];
            // Identity matrix
            for (int i = 0; i < 16; ++i) {
                mtx[i] = (i % 5 == 0) ? 1.0f : 0.0f;
            }

            bgfx::setTransform(mtx);
            bgfx::setVertexBuffer(0, vbh);
            bgfx::setIndexBuffer(ibh);
            bgfx::setState(BGFX_STATE_DEFAULT);
            bgfx::submit(0, program);
        }

        // Advance to next frame and let bgfx render.
        bgfx::frame();

        glfwPollEvents();
        usleep(10000);  // 10 ms
    }

    // Cleanup resources.
    if (bgfx::isValid(vbh)) {
        bgfx::destroy(vbh);
    }
    if (bgfx::isValid(ibh)) {
        bgfx::destroy(ibh);
    }
    if (bgfx::isValid(program)) {
        bgfx::destroy(program);
    }

    bgfx::shutdown();

    glfwDestroyWindow(window);
}
