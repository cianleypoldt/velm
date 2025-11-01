#include "velm/velm.h"

#include "bgfx/bgfx.h"
#include "bx/platform.h"

#if BX_PLATFORM_LINUX
#    define GLFW_EXPOSE_NATIVE_X11
#    define GLFW_EXPOSE_NATIVE_WAYLAND
#elif BX_PLATFORM_WINDOWS
#    define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
#    define GLFW_EXPOSE_NATIVE_COCOA
#elif BX_PLATFORM_BSD
#    define GLFW_EXPOSE_NATIVE_X11
#endif
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

velm::Velm::Velm() {
    glfwInit();
    GLFWwindow * window = glfwCreateWindow(800, 600, "debug_view", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);  // required for OpenGL backend

    bgfx::Init init;
    init.type = bgfx::RendererType::Count;

#if BX_PLATFORM_LINUX
    init.platformData.nwh = (void *) (uintptr_t) glfwGetX11Window(window);
    init.platformData.ndt = glfwGetX11Display();
#elif BX_PLATFORM_BSD
    init.platformData.nwh = glfwGetX11Window(window);
    init.platformData.ndt = glfwGetX11Display();
#elif BX_PLATFORM_OSX
    init.platformData.nwh = glfwGetCocoaWindow(window);
#elif BX_PLATFORM_WINDOWS
    init.platformData.nwh = glfwGetWin32Window(window);
#else
    abort();
#endif

    init.resolution.width  = 800;
    init.resolution.height = 600;

    if (!bgfx::init(init)) {
        printf("BGFX Init failed.\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return;
    }

    velm_shadersys::load_all();
}

velm::Velm::~Velm() {
    velm_shadersys::destroy_all();
    bgfx::shutdown();
}
