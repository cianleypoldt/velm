#include "velm/window.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <unistd.h>

void test_namespace::open_and_close() {
    if (!glfwInit()) {
        return;
    }

    // Request an OpenGL context so we can render via bgfx on top of GLFW window.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor *       monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode * mode    = monitor ? glfwGetVideoMode(monitor) : nullptr;

    int          width  = mode ? mode->width : 800;
    int          height = mode ? mode->height : 600;
    GLFWwindow * window = glfwCreateWindow(width, height, "hyprland-fullscreen", NULL, nullptr);
    if (!window) {
        glfwTerminate();
        return;
    }

    // Ensure no window decorations (some compositors respect this for "clean" fullscreen).
    glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);

    // Make the context current and enable vsync for sane CPU usage.
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    //                    Main loop: clear to bla                c    k each frame and poll events until the OS requests close.
    while (!glfwWindowShouldClose(window)) {
        // Make sure the viewport matches the current window size (handles compositor resize).
        int fbw = 0, fbh = 0;
        glfwGetFramebufferSize(window, &fbw, &fbh);
        if (fbw > 0 && fbh > 0) {
            glViewport(0, 0, fbw, fbh);
        }

        // Clear the screen to black.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Present and handle events.
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
}
